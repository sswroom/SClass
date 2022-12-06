#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/HEIFParser.h"

Parser::FileParser::HEIFParser::HEIFParser()
{
}

Parser::FileParser::HEIFParser::~HEIFParser()
{
}

Int32 Parser::FileParser::HEIFParser::GetName()
{
	return *(Int32*)"HEIF";
}

void Parser::FileParser::HEIFParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.heic"), CSTR("HEIC"));
		selector->AddFilter(CSTR("*.heif"), CSTR("HEIF"));
		selector->AddFilter(CSTR("*.avif"), CSTR("AVIF"));
	}
}

IO::ParserType Parser::FileParser::HEIFParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

IO::ParsedObject *Parser::FileParser::HEIFParser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	return 0;
}
