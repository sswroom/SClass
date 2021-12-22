#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/DebugWriter.h"

IO::DebugWriter::DebugWriter()
{
}

IO::DebugWriter::~DebugWriter()
{
}

void IO::DebugWriter::Flush()
{
}

Bool IO::DebugWriter::Write(const UTF8Char *str, UOSInt nChar)
{
	return true;
}

Bool IO::DebugWriter::Write(const UTF8Char *str)
{
	return true;
}

Bool IO::DebugWriter::WriteLine(const UTF8Char *str, UOSInt nChar)
{
	return true;
}

Bool IO::DebugWriter::WriteLine(const UTF8Char *str)
{
	return true;
}

Bool IO::DebugWriter::WriteLine()
{
	return true;
}
