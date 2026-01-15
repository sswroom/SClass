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

Bool IO::ConsoleWriter::WriteStrC(const UTF8Char *str, UIntOS nChar)
{
	while (nChar > 0)
	{
		SerialPort_Write(*str++);
		nChar--;
	}
	return true;
}

Bool IO::ConsoleWriter::WriteLineC(const UTF8Char *str, UIntOS nChar)
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
		
UIntOS IO::ConsoleWriter::CalDisplaySize(const WChar *str)
{
	return 0;
}

WChar *IO::ConsoleWriter::ReadLine(WChar *sbuff, UIntOS nChar)
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

void IO::ConsoleWriter::FixWrite(const WChar *str, UIntOS displayWidth)
{
}

UIntOS IO::ConsoleWriter::GetDisplayWidth(const WChar *str)
{
	return 0;
}

UIntOS IO::ConsoleWriter::GetDisplayCharWidth(WChar c)
{
	return 1;
}

