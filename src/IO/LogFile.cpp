#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/LogFile.h"

IO::LogFile::LogFile(const UTF8Char *sourceName) : IO::ParsedObject(sourceName)
{
}

IO::LogFile::~LogFile()
{
}

IO::ParserType IO::LogFile::GetParserType()
{
	return IO::ParserType::LogFile;
}

