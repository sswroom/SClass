#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DebugWriter.h"

IO::DebugWriter::DebugWriter()
{
	this->clsData = 0;
}

IO::DebugWriter::~DebugWriter()
{
}

void IO::DebugWriter::Flush()
{
}

Bool IO::DebugWriter::WriteStrC(const UTF8Char *str, UOSInt nChar)
{
	return true;
}

Bool IO::DebugWriter::WriteLineC(const UTF8Char *str, UOSInt nChar)
{
	return true;
}

Bool IO::DebugWriter::WriteLine()
{
	return true;
}
