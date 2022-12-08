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

void Parser::FileParser::JP2Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
}

IO::ParserType Parser::FileParser::JP2Parser::GetParserType()
{
	return IO::ParserType::ImageList;
}

IO::ParsedObject *Parser::FileParser::JP2Parser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	return 0;
}
