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

Bool IO::DebugWriter::Write(Text::CStringNN str)
{
	return true;
}

Bool IO::DebugWriter::WriteLine(Text::CStringNN str)
{
	return true;
}

Bool IO::DebugWriter::WriteLine()
{
	return true;
}
