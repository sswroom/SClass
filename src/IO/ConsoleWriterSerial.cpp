#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

extern "C"
{
	void SerialPort_Init(Int32 baudRate);
	void SerialPort_Write(UInt8 b);
}

IO::ConsoleWriter::ConsoleWriter()
{
	SerialPort_Init(57600);
}

IO::ConsoleWriter::~ConsoleWriter()
{
}

Bool IO::ConsoleWriter::Write(const UTF8Char *str, OSInt nChar)
{
	while (nChar > 0)
	{
		SerialPort_Write(*str++);
		nChar--;
	}
	return true;
}

Bool IO::ConsoleWriter::Write(const UTF8Char *str)
{
	UInt8 b;
	while ((b = *str++) != 0)
	{
		SerialPort_Write(b);
	}
	return true;
}

Bool IO::ConsoleWriter::WriteLine(const UTF8Char *str, OSInt nChar)
{
	while (nChar > 0)
	{
		SerialPort_Write(*str++);
		nChar--;
	}
	SerialPort_Write(13);
	SerialPort_Write(10);
	return true;
}

Bool IO::ConsoleWriter::WriteLine(const UTF8Char *str)
{
	UInt8 b;
	while ((b = *str++) != 0)
	{
		SerialPort_Write(b);
	}
	SerialPort_Write(13);
	SerialPort_Write(10);
	return true;
}

Bool IO::ConsoleWriter::WriteW(const WChar *str, OSInt nChar)
{
#if _WCHAR_SIZE == 1
	while (nChar > 0)
	{
		SerialPort_Write(*str++);
		nChar--;
	}
	return true;
#elif _WCHAR_SIZE == 2
	WChar c;
	UTF32Char code;
	while (nChar > 0)
	{
		c = *str++;
		if (c < 0x80)
		{
			SerialPort_Write((UInt8)c);
		}
		else if (c < 0x800)
		{
			SerialPort_Write(0xc0 | (c >> 6));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		else if (c >= 0xd800 && c < 0xdc00 && nChar > 1 && str[0] >= 0xdc00 && str[0] < 0xe000)
		{
			code = 0x10000 + ((UTF32Char)(c - 0xd800) << 10) + (str[0] - 0xdc00);
			str++;
			nChar--;
			if (code < 0x200000)
			{
				SerialPort_Write(0xf0 | (code >> 18));
				SerialPort_Write(0x80 | ((code >> 12) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 6) & 0x3f));
				SerialPort_Write(0x80 | (code & 0x3f));
			}
			else if (code < 0x4000000)
			{
				SerialPort_Write(0xf8 | (code >> 24));
				SerialPort_Write(0x80 | ((code >> 18) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 12) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 6) & 0x3f));
				SerialPort_Write(0x80 | (code & 0x3f));
			}
			else
			{
				SerialPort_Write(0xfc | (code >> 30));
				SerialPort_Write(0x80 | ((code >> 24) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 18) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 12) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 6) & 0x3f));
				SerialPort_Write(0x80 | (code & 0x3f));
			}
		}
		else
		{
			SerialPort_Write(0xe0 | (c >> 12));
			SerialPort_Write(0x80 | ((c >> 6) & 0x3f));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		nChar--;
	}
	return true;
#elif _WCHAR_SIZE == 4
	WChar c;
	UTF32Char code;
	while (nChar-- > 0)
	{
		c = *str++;
		if (c < 0x80)
		{
			SerialPort_Write((UInt8)c);
		}
		else if (c < 0x800)
		{
			SerialPort_Write(0xc0 | (c >> 6));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		else if (c < 0x10000)
		{
			SerialPort_Write(0xe0 | (c >> 12));
			SerialPort_Write(0x80 | ((c >> 6) & 0x3f));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		else if (c < 0x200000)
		{
			SerialPort_Write(0xf0 | (c >> 18));
			SerialPort_Write(0x80 | ((c >> 12) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 6) & 0x3f));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		else if (c < 0x4000000)
		{
			SerialPort_Write(0xf8 | (c >> 24));
			SerialPort_Write(0x80 | ((c >> 18) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 12) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 6) & 0x3f));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		else
		{
			SerialPort_Write(0xfc | (c >> 30));
			SerialPort_Write(0x80 | ((c >> 24) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 18) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 12) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 6) & 0x3f));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
	}
	return true;
#else
	return false;
#endif
}

Bool IO::ConsoleWriter::WriteW(const WChar *str)
{
#if _WCHAR_SIZE == 1
	UInt8 b;
	while ((b = *str++) != 0)
	{
		SerialPort_Write(b);
	}
	return true;
#elif _WCHAR_SIZE == 2
	WChar c;
	UTF32Char code;
	while (true)
	{
		c = *str++;
		if (c == 0)
		{
			break;
		}
		else if (c < 0x80)
		{
			SerialPort_Write((UInt8)c);
		}
		else if (c < 0x800)
		{
			SerialPort_Write(0xc0 | (c >> 6));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		else if (c >= 0xd800 && c < 0xdc00 && str[0] >= 0xdc00 && str[0] < 0xe000)
		{
			code = 0x10000 + ((UTF32Char)(c - 0xd800) << 10) + (str[0] - 0xdc00);
			str++;
			if (code < 0x200000)
			{
				SerialPort_Write(0xf0 | (code >> 18));
				SerialPort_Write(0x80 | ((code >> 12) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 6) & 0x3f));
				SerialPort_Write(0x80 | (code & 0x3f));
			}
			else if (code < 0x4000000)
			{
				SerialPort_Write(0xf8 | (code >> 24));
				SerialPort_Write(0x80 | ((code >> 18) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 12) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 6) & 0x3f));
				SerialPort_Write(0x80 | (code & 0x3f));
			}
			else
			{
				SerialPort_Write(0xfc | (code >> 30));
				SerialPort_Write(0x80 | ((code >> 24) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 18) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 12) & 0x3f));
				SerialPort_Write(0x80 | ((code >> 6) & 0x3f));
				SerialPort_Write(0x80 | (code & 0x3f));
			}
		}
		else
		{
			SerialPort_Write(0xe0 | (c >> 12));
			SerialPort_Write(0x80 | ((c >> 6) & 0x3f));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
	}
	return true;
#elif _WCHAR_SIZE == 4
	WChar c;
	UTF32Char code;
	while (true)
	{
		if ((c = *str++) == 0)
		{
			break;
		}
		else if (c < 0x80)
		{
			SerialPort_Write((UInt8)c);
		}
		else if (c < 0x800)
		{
			SerialPort_Write(0xc0 | (c >> 6));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		else if (c < 0x10000)
		{
			SerialPort_Write(0xe0 | (c >> 12));
			SerialPort_Write(0x80 | ((c >> 6) & 0x3f));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		else if (c < 0x200000)
		{
			SerialPort_Write(0xf0 | (c >> 18));
			SerialPort_Write(0x80 | ((c >> 12) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 6) & 0x3f));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		else if (c < 0x4000000)
		{
			SerialPort_Write(0xf8 | (c >> 24));
			SerialPort_Write(0x80 | ((c >> 18) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 12) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 6) & 0x3f));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
		else
		{
			SerialPort_Write(0xfc | (c >> 30));
			SerialPort_Write(0x80 | ((c >> 24) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 18) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 12) & 0x3f));
			SerialPort_Write(0x80 | ((c >> 6) & 0x3f));
			SerialPort_Write(0x80 | (c & 0x3f));
		}
	}
	return true;
#else
	return false;
#endif
}

Bool IO::ConsoleWriter::WriteLineW(const WChar *str, OSInt nChar)
{
	this->WriteW(str, nChar);
	SerialPort_Write(13);
	SerialPort_Write(10);
	return true;
}

Bool IO::ConsoleWriter::WriteLineW(const WChar *str)
{
	this->WriteW(str);
	SerialPort_Write(13);
	SerialPort_Write(10);
	return true;
}

Bool IO::ConsoleWriter::WriteLine()
{
	SerialPort_Write(13);
	SerialPort_Write(10);
	return true;
}

void IO::ConsoleWriter::SetTextColor(UInt8 fgColor, UInt8 bgColor)
{
}

void IO::ConsoleWriter::ResetTextColor()
{
}
		
OSInt IO::ConsoleWriter::CalDisplaySize(const WChar *str)
{
	return 0;
}

WChar *IO::ConsoleWriter::ReadLine(WChar *sbuff, OSInt nChar)
{
	return 0;
}

void IO::ConsoleWriter::EnableCPFix(Bool isEnable)
{
}

void IO::ConsoleWriter::SetAutoFlush(Bool autoFlush)
{
}

Bool IO::ConsoleWriter::GetConsoleState(IO::ConsoleWriter::ConsoleState *state)
{
	return false;
}

Bool IO::ConsoleWriter::SetCursorPos(Int32 x, Int32 y)
{
	return false;
}

Bool IO::ConsoleWriter::IsFileOutput()
{
	return false;
}

void IO::ConsoleWriter::FixWrite(const WChar *str, OSInt displayWidth)
{
}

OSInt IO::ConsoleWriter::GetDisplayWidth(const WChar *str)
{
	return 0;
}

OSInt IO::ConsoleWriter::GetDisplayCharWidth(WChar c)
{
	return 1;
}

