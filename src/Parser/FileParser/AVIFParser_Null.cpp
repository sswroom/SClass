#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/AVIFParser.h"

Parser::FileParser::AVIFParser::AVIFParser()
{
}

Parser::FileParser::AVIFParser::~AVIFParser()
{
}

Int32 Parser::FileParser::AVIFParser::GetName()
{
	return *(Int32*)"AVIF";
}

void Parser::FileParser::AVIFParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.avif"), CSTR("AVIF"));
	}
}

IO::ParserType Parser::FileParser::AVIFParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::AVIFParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	return nullptr;
}

Bool Parser::FileParser::AVIFParser::ParseHeaders(NN<IO::StreamData> fd, OutParam<Optional<Media::EXIFData>> exif, OutParam<Optional<Text::XMLDocument>> xmf, OutParam<Optional<Media::ICCProfile>> icc, OutParam<UInt32> width, OutParam<UInt32> height)
{
	return false;
}