#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/JXLParser.h"

Parser::FileParser::JXLParser::JXLParser()
{
}

Parser::FileParser::JXLParser::~JXLParser()
{
}

Int32 Parser::FileParser::JXLParser::GetName()
{
	return *(Int32*)"JXLP";
}

void Parser::FileParser::JXLParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
}

IO::ParserType Parser::FileParser::JXLParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::JXLParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	return nullptr;
}
