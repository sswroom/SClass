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

void Parser::FileParser::WebPParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::WebPParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	return 0;
}
