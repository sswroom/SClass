#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/JP2Parser.h"

Parser::FileParser::JP2Parser::JP2Parser()
{
}

Parser::FileParser::JP2Parser::~JP2Parser()
{
}

Int32 Parser::FileParser::JP2Parser::GetName()
{
	return *(Int32*)"JP2P";
}

void Parser::FileParser::JP2Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
}

IO::ParserType Parser::FileParser::JP2Parser::GetParserType()
{
	return IO::ParserType::ImageList;
}

IO::ParsedObject *Parser::FileParser::JP2Parser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	return 0;
}
