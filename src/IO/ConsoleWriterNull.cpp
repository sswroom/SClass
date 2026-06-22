#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

IO::ConsoleWriter::ConsoleWriter()
{
	this->clsData = 0;
}

IO::ConsoleWriter::~ConsoleWriter()
{
}

Bool IO::ConsoleWriter::Write(Text::CStringNN str)
{
	return false;
}

Bool IO::ConsoleWriter::WriteLine(Text::CStringNN str)
{
	return false;
}

Bool IO::ConsoleWriter::WriteLine()
{
	return false;
}

void IO::ConsoleWriter::SetBGColor(Text::StandardColor bgColor)
{
}

void IO::ConsoleWriter::SetTextColor(Text::StandardColor fgColor)
{
}

void IO::ConsoleWriter::ResetTextColor()
{
}
		
UIntOS IO::ConsoleWriter::CalDisplaySize(const WChar *str)
{
	return 0;
}

UnsafeArrayOpt<WChar> IO::ConsoleWriter::ReadLine(UnsafeArray<WChar> sbuff, UIntOS nChar)
{
	return nullptr;
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

