#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/LogFile.h"

IO::LogFile::LogFile(Text::String *sourceName) : IO::ParsedObject(sourceName)
{
}

IO::LogFile::~LogFile()
{
}

IO::ParserType IO::LogFile::GetParserType() const
{
	return IO::ParserType::LogFile;
}

