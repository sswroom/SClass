#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

#if defined(__sun__)
#include <unistd.h>
#endif
#include <stdio.h>
#include <termios.h>
#include <wchar.h>
#include <sys/ioctl.h>

IO::ConsoleWriter::ConsoleWriter()
{
	setbuf(stdout, 0);
	this->clsData = 0;
	this->bgColor = Text::StandardColor::Black;
}

IO::ConsoleWriter::~ConsoleWriter()
{
}

Bool IO::ConsoleWriter::Write(Text::CStringNN str)
{
	printf("%s", str.v.Ptr());
	return true;
}

Bool IO::ConsoleWriter::WriteLine(Text::CStringNN str)
{
	printf("%s\n", str.v.Ptr());
	return true;
}

Bool IO::ConsoleWriter::WriteLine()
{
	printf("\n");
	return true;
}

Bool IO::ConsoleWriter::WriteChar(UTF8Char c)
{
	printf("%c", c);
	return true;
}

void IO::ConsoleWriter::SetBGColor(Text::StandardColor bgColor)
{
	this->bgColor = bgColor;
}

void IO::ConsoleWriter::SetTextColor(Text::StandardColor fgCol)
{
	UInt8 fgColor = (UInt8)fgCol;
	UInt8 bgColor = (UInt8)this->bgColor;
	fgColor = (UInt8)((fgColor & ~5) | ((fgColor & 1) << 2) | ((fgColor & 4) >> 2));
	bgColor = (UInt8)((bgColor & ~5) | ((bgColor & 1) << 2) | ((bgColor & 4) >> 2));
	if (fgColor & 8)
	{
		printf("\x1b[1;3%i;4%im", fgColor & 7, bgColor & 7);
	}
	else
	{
		printf("\x1b[0;3%i;4%im", fgColor & 7, bgColor & 7);
	}
}

void IO::ConsoleWriter::ResetTextColor()
{
	printf("\x1b[0m");
}
		
UOSInt IO::ConsoleWriter::CalDisplaySize(const WChar *str)
{
	return 0;
}

WChar *IO::ConsoleWriter::ReadLine(WChar *sbuff, UOSInt nChar)
{
#if !defined(__arm__)
	if (fgetws(sbuff, (int)nChar, stdin) == 0)
		return 0;
	else
	{
		while (*sbuff++);
		return sbuff - 1;
	}
#else
	return 0;
#endif
}

void IO::ConsoleWriter::EnableCPFix(Bool isEnable)
{
}

void IO::ConsoleWriter::SetAutoFlush(Bool autoFlush)
{
}

Bool IO::ConsoleWriter::GetConsoleState(IO::ConsoleWriter::ConsoleState *state)
{
/*			UInt8 fgColor;
			UInt8 bgColor;
			Int32 currX;
			Int32 currY;
			Int32 consoleWidth;
			Int32 consoleHeight;*/

	Char row[32];
	OSInt i;
	Char col[32];
	Bool succ = false;
	Int32 c;
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	state->consoleWidth = w.ws_col;
	state->consoleHeight = w.ws_row;
	state->fgColor = Text::StandardColor::Black;
	state->bgColor = Text::StandardColor::Black;

	struct termios oldIOS;
	struct termios newIOS;
	tcgetattr(0, &oldIOS);
	newIOS = oldIOS;
	newIOS.c_lflag = newIOS.c_lflag & (tcflag_t)~(ICANON | ECHO);
	tcsetattr(0, TCSANOW, &newIOS);

	printf("\x1b[6n");
	while (true)
	{
		c = getchar();
		if (c ==  0x1b)
		{
			break;
		}
	}
	if (getchar() == '[')
	{
		succ = true;
		i = 0;
		while (true)
		{
			c = getchar();
			if (c >= '0' && c <= '9')
			{
				row[i++] = (Char)c;
			}
			else if (c == ';')
			{
				break;
			}
			else
			{
				succ = false;
			}
		}
		if (succ)
		{
			row[i] = 0;
			i = 0;
			while (true)
			{
				c = getchar();
				if (c >= '0' && c <= '9')
				{
					col[i++] = (Char)c;
				}
				else if (c == 'R')
				{
					break;
				}
				else
				{
					succ = false;
				}
			}
		}
		if (succ)
		{
			col[i] = 0;
			state->currX = Text::StrToUInt32Ch(col) - 1;
			state->currY = Text::StrToInt32Ch(row) - 1;
		}
	}
	tcsetattr(0, TCSANOW, &oldIOS);
	return succ;
}

Bool IO::ConsoleWriter::SetCursorPos(UInt32 x, Int32 y)
{
	printf("\x1b[%d;%dH", y + 1, x + 1);
	return true;
}

Bool IO::ConsoleWriter::IsFileOutput()
{
	return false;
}

void IO::ConsoleWriter::FixWrite(const WChar *str, UOSInt displayWidth)
{
}

UOSInt IO::ConsoleWriter::GetDisplayWidth(const WChar *str)
{
	return 80;
}

UOSInt IO::ConsoleWriter::GetDisplayCharWidth(WChar c)
{
	if (c < 128)
		return 1;

	return 2;
}
