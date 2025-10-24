#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Encoding.h"
#include "IO/Console.h"

#include <stdio.h>
#include <termios.h>
#include <wchar.h>
#include <sys/poll.h>

IO::Console::ConsoleHandler IO::Console::exitCmdHdlr = 0;
AnyType IO::Console::exitCmdObj = 0;
Int32 Console_nextKey = 0;

Int32 __stdcall IO::Console::ConsoleHdlr(UInt32 dwCtrlType)
{
	if (dwCtrlType == 2)
	{
		if (exitCmdHdlr)
		{
			exitCmdHdlr(exitCmdObj);
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

UTF32Char IO::Console::GetChar()
{
	return (UTF32Char)getwchar();
}

void IO::Console::PutChar(UTF32Char c)
{
#if _WCHAR_SIZE == 4
	ungetwc((wint_t)c, stdin);
#else
	if (c >= 0x10000)
	{
		ungetwc((0xd800 + (c >> 10), stdin);
		ungetwc((c & 0x3ff) + 0xdc00, stdin);
	}
	else
	{
		ungetwc((WChar)c, stdin);
	}
#endif
}

UnsafeArray<UTF8Char> IO::Console::GetLine(UnsafeArray<UTF8Char> buff)
{
	UnsafeArray<UTF8Char> ptr = buff;
	UTF32Char c;
	while (true)
	{
		c = (UTF32Char)getwchar();
		if (c == 13 || c == 10)
		{
			*ptr = 0;
			return ptr;
		}

		if (c < 0x80)
		{
			*ptr++ = (UInt8)c;
		}
		else if (c < 0x800)
		{
			*ptr++ = (UTF8Char)(0xc0 | (c >> 6));
			*ptr++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (c < 0x10000)
		{
			*ptr++ = (UTF8Char)(0xe0 | (c >> 12));
			*ptr++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*ptr++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (c < 0x200000)
		{
			*ptr++ = (UTF8Char)(0xf0 | (c >> 18));
			*ptr++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
			*ptr++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*ptr++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else if (c < 0x4000000)
		{
			*ptr++ = (UTF8Char)(0xf8 | (c >> 24));
			*ptr++ = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
			*ptr++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
			*ptr++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*ptr++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
		else
		{
			*ptr++ = (UTF8Char)(0xfc | (c >> 30));
			*ptr++ = (UTF8Char)(0x80 | ((c >> 24) & 0x3f));
			*ptr++ = (UTF8Char)(0x80 | ((c >> 18) & 0x3f));
			*ptr++ = (UTF8Char)(0x80 | ((c >> 12) & 0x3f));
			*ptr++ = (UTF8Char)(0x80 | ((c >> 6) & 0x3f));
			*ptr++ = (UTF8Char)(0x80 | (c & 0x3f));
		}
	}
}

void IO::Console::PrintStrO(UnsafeArray<const UTF8Char> str1)
{
	printf("%s", str1.Ptr());
	fflush(stdout);
}

UOSInt IO::Console::WriteStdOut(UnsafeArray<const UInt8> buff, UOSInt size)
{
	return fwrite(buff.Ptr(), 1, size, stdout);
}

void IO::Console::PreventCtrlC()
{
}

void IO::Console::HandleExitCmd(IO::Console::ConsoleHandler exitCmdHdlr, AnyType userObj)
{
}

void IO::Console::SetCodePage(UInt32 codePage)
{
}

Int32 IO::Console::GetKey()
{
	Int32 k;
	if (Console_nextKey != 0)
	{
		k = Console_nextKey;
		Console_nextKey = 0;
		return k;
	}
	struct termios oldIOS;
	struct termios newIOS;
	tcgetattr(0, &oldIOS);
	newIOS = oldIOS;
	newIOS.c_lflag &= (tcflag_t)~(ICANON | ECHO);
	tcsetattr(0, TCSANOW, &newIOS);

	k = getchar();
	if (k == 0x1b)
	{
		newIOS.c_lflag &= (tcflag_t)~ISIG;
		newIOS.c_cc[VMIN] = 0;
		newIOS.c_cc[VTIME] = 0;
		tcsetattr(0, TCSANOW, &newIOS);

		k = getchar();
		if (k == EOF)
		{
			k = 0x1b;
		}
		else if (k == 0x5b)
		{
			k = getchar();
			if (k == 0x41) //Up
			{
				Console_nextKey = 0x48;
				k = 0;
			}
			else if (k == 0x42) //Down
			{
				Console_nextKey = 0x50;
				k = 0;
			}
			else if (k == 0x43) //Right
			{
				Console_nextKey = 0x4d;
				k = 0;
			}
			else if (k == 0x44) //Left
			{
				Console_nextKey = 0x4b;
				k = 0;
			}
			else
			{
				wprintf(L"1b 5b %x\r\n", k);
			}
		}
		else
		{
			wprintf(L"1b %x\r\n", k);
		}
	}
	else if (k == 0xa)
	{
		k = 0xd;
	}

	tcsetattr(0, TCSANOW, &oldIOS);
	return k;
}

