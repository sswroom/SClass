#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Parser/FileParser/EVTXParser.h"

//https://github.com/libyal/libevtx/blob/main/documentation/Windows%20XML%20Event%20Log%20(EVTX).asciidoc

Parser::FileParser::EVTXParser::EVTXParser()
{
}

Parser::FileParser::EVTXParser::~EVTXParser()
{
}

Int32 Parser::FileParser::EVTXParser::GetName()
{
	return *(Int32*)"EVTX";
}

void Parser::FileParser::EVTXParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::LogFile)
	{
		selector->AddFilter(CSTR("*.evtx"), CSTR("EVTX File"));
	}
}

IO::ParserType Parser::FileParser::EVTXParser::GetParserType()
{
	return IO::ParserType::LogFile;
}

IO::ParsedObject *Parser::FileParser::EVTXParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	return 0;
}
