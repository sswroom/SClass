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

void Parser::FileParser::HEIFParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::HEIFParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	return 0;
}

Bool Parser::FileParser::HEIFParser::ParseHeaders(NN<IO::StreamData> fd, OutParam<Optional<Media::EXIFData>> exif, OutParam<Optional<Text::XMLDocument>> xmf, OutParam<Optional<Media::ICCProfile>> icc, OutParam<UInt32> width, OutParam<UInt32> height)
{
	return false;
}

