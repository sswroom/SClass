#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/LNKParser.h"
#include "IO/PackageFile.h"

Parser::FileParser::LNKParser::LNKParser()
{
}

Parser::FileParser::LNKParser::~LNKParser()
{
}

Int32 Parser::FileParser::LNKParser::GetName()
{
	return *(Int32*)"LNKP";
}

void Parser::FileParser::LNKParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.lnk"), CSTR("Link File"));
	}
}

IO::ParserType Parser::FileParser::LNKParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::LNKParser::ParseFile(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	return 0;
}
