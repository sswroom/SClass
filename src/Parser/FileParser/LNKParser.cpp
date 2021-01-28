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

void Parser::FileParser::LNKParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.lnk", (const UTF8Char*)"Link File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::LNKParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::LNKParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	return 0;
}
