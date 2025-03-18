#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/JPGParser.h"

Parser::FileParser::JPGParser::JPGParser()
{
}

Parser::FileParser::JPGParser::~JPGParser()
{
}

Int32 Parser::FileParser::JPGParser::GetName()
{
	return *(Int32*)"JPGP";
}

void Parser::FileParser::JPGParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
}

IO::ParserType Parser::FileParser::JPGParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::JPGParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	return 0;
}
