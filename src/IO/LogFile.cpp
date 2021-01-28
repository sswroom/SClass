#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/LogFile.h"

IO::LogFile::LogFile(const UTF8Char *sourceName) : IO::ParsedObject(sourceName)
{
}

IO::LogFile::~LogFile()
{
}

IO::ParsedObject::ParserType IO::LogFile::GetParserType()
{
	return IO::ParsedObject::PT_LOG_FILE;
}

