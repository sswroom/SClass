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

void Parser::FileParser::IMGParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::IMGParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 buff[512];
	if (fd->GetRealData(0, 512, buff) != 512)
		return 0;
	if (buff[0] == 0xeb && buff[510] == 0x55 && buff[511] == 0xaa && (fd->GetDataSize() & 511) == 0)
	{
		IO::FileSectorData *data;
		NEW_CLASS(data, IO::FileSectorData(fd, 0, fd->GetDataSize(), 512));
		return data;
	}
	return 0;
}
