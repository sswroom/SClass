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

Bool IO::ConsoleWriter::WriteStrC(const UTF8Char *str, UOSInt nChar)
{
	while (nChar > 0)
	{
		SerialPort_Write(*str++);
		nChar--;
	}
	return true;
}

Bool IO::ConsoleWriter::WriteLineC(const UTF8Char *str, UOSInt nChar)
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

Bool IO::ConsoleWriter::WriteLine()
{
	SerialPort_Write(13);
	SerialPort_Write(10);
	return true;
}

void IO::ConsoleWriter::SetTextColor(IO::ConsoleWriter::ConsoleColor fgColor, IO::ConsoleWriter::ConsoleColor bgColor)
{
}

void IO::ConsoleWriter::ResetTextColor()
{
}
		
UOSInt IO::ConsoleWriter::CalDisplaySize(const WChar *str)
{
	return 0;
}

WChar *IO::ConsoleWriter::ReadLine(WChar *sbuff, UOSInt nChar)
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

Bool IO::ConsoleWriter::SetCursorPos(UInt32 x, Int32 y)
{
	return false;
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
	return 0;
}

UOSInt IO::ConsoleWriter::GetDisplayCharWidth(WChar c)
{
	return 1;
}

