#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

IO::ConsoleWriter::ConsoleWriter()
{
}

IO::ConsoleWriter::~ConsoleWriter()
{
}

Bool IO::ConsoleWriter::Write(const UTF8Char *str, OSInt nChar)
{
	return false;
}

Bool IO::ConsoleWriter::Write(const UTF8Char *str)
{
	return false;
}

Bool IO::ConsoleWriter::WriteLine(const UTF8Char *str, OSInt nChar)
{
	return false;
}

Bool IO::ConsoleWriter::WriteLine(const UTF8Char *str)
{
	return false;
}

Bool IO::ConsoleWriter::Write(const WChar *str, OSInt nChar)
{
	return false;
}

Bool IO::ConsoleWriter::Write(const WChar *str)
{
	return false;
}

Bool IO::ConsoleWriter::WriteLine(const WChar *str, OSInt nChar)
{
	return false;
}

Bool IO::ConsoleWriter::WriteLine(const WChar *str)
{
	return false;
}

Bool IO::ConsoleWriter::WriteLine()
{
	return false;
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

