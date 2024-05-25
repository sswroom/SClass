#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileSectorData.h"
#include "Parser/FileParser/IMGParser.h"

Parser::FileParser::IMGParser::IMGParser()
{
}

Parser::FileParser::IMGParser::~IMGParser()
{
}

Int32 Parser::FileParser::IMGParser::GetName()
{
	return *(Int32*)"IMGP";
}

void Parser::FileParser::IMGParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::SectorData)
	{
		selector->AddFilter(CSTR("*.img"), CSTR("IMG (Floppy) File"));
	}
}

IO::ParserType Parser::FileParser::IMGParser::GetParserType()
{
	return IO::ParserType::SectorData;
}

Optional<IO::ParsedObject> Parser::FileParser::IMGParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (hdr[0] == 0xeb && hdr[510] == 0x55 && hdr[511] == 0xaa && (fd->GetDataSize() & 511) == 0)
	{
		IO::FileSectorData *data;
		NEW_CLASS(data, IO::FileSectorData(fd, 0, fd->GetDataSize(), 512));
		return data;
	}
	return 0;
}
