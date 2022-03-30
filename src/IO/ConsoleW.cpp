#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Console.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#if (defined(WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(_WIN32)) && !defined(_WIN32_WCE)
#if defined(__CYGWIN__)
#include <wchar.h>
#else
#include <stdio.h>
#include <conio.h>
#endif
#include <io.h>
#include <windows.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>

IO::Console::ConsoleHandler IO::Console::exitCmdHdlr = 0;
void *IO::Console::exitCmdObj = 0;

Int32 __stdcall IO::Console::ConsoleHdlr(UInt32 dwCtrlType)
{
	if (dwCtrlType == 2)
	{
		if (exitCmdHdlr)
		{
			exitCmdHdlr(exitCmdObj);
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UTF32Char IO::Console::GetChar()
{
	return getwchar();
}

void IO::Console::PutChar(UTF32Char c)
{
	if (c >= 0x10000)
	{
		ungetwc((wint_t)(0xd800 + (c >> 10)), stdin);
		ungetwc((wint_t)((c & 0x3ff) + 0xdc00), stdin);
	}
	else
	{
		ungetwc((wint_t)c, stdin);
	}
}

UTF8Char *IO::Console::GetLine(UTF8Char *buff)
{
#if defined(__CYGWIN__)
	Char sbuff[512];
    Char *ptr;
    ptr = gets(sbuff);
	if (ptr)
	{
		return Text::StrConcat(buff, (UTF8Char*)sbuff);
	}
	else
	{
		*buff = 0;
		return buff;
	}
#else
	WChar wbuff[256];
	WChar *ptr;
#if _MSC_VER >= 1400
	ptr = _getws_s(wbuff, 256);
#else
	ptr = _getws(wbuff);
#endif
	if (ptr)
	{
		return Text::StrWChar_UTF8(buff, wbuff);
	}
	else
	{
		*buff = 0;
		return buff;
	}
#endif
}

void IO::Console::PrintStrO(const UTF8Char *str1)
{
	const WChar *wptr = Text::StrToWCharNew(str1);
	wprintf(L"%ls", wptr);
	Text::StrDelNew(wptr);
	fflush(stdout);
}

UOSInt IO::Console::WriteStdOut(UInt8 *buff, UOSInt size)
{
#if defined(WIN32) && !defined(_WIN32_WCE) && !defined(__CYGWIN__)
	Int32 ret = _write(1, buff, (UInt32)size);
	if (ret < 0)
	{
		return 0;
	}
	else
	{
		return (UOSInt)ret;
	}
#else
	return fwrite(buff, 1, size, stdout);
#endif
}

void IO::Console::PreventCtrlC()
{
#if (defined(WIN32) || defined(__CYGWIN__)) && !defined(_WIN32_WCE)
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHdlr, TRUE);
#endif
}

void IO::Console::HandleExitCmd(IO::Console::ConsoleHandler exitCmdHdlr, void *userObj)
{
#if (defined(WIN32) || defined(__CYGWIN__)) && !defined(_WIN32_WCE)
	IO::Console::exitCmdHdlr = exitCmdHdlr;
	IO::Console::exitCmdObj = userObj;
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHdlr, TRUE);
#endif
}

void IO::Console::SetCodePage(UInt32 codePage)
{
#if (defined(WIN32) || defined(__CYGWIN__)) && !defined(_WIN32_WCE)
	SetConsoleOutputCP(codePage);
#endif
}

Int32 IO::Console::GetKey()
{
#if !defined(__CYGWIN__) && !defined(_WIN32_WCE)
	return _getch();
#else
	return getchar();
#endif
}

