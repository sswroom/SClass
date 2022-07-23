#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/WebPParser.h"

Parser::FileParser::WebPParser::WebPParser()
{
}

Parser::FileParser::WebPParser::~WebPParser()
{
}

Int32 Parser::FileParser::WebPParser::GetName()
{
	return *(Int32*)"WEBP";
}

void Parser::FileParser::WebPParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.webp"), CSTR("WebP"));
	}
}

IO::ParserType Parser::FileParser::WebPParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

IO::ParsedObject *Parser::FileParser::WebPParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	return 0;
}
