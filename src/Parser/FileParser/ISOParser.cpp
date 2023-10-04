#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileSectorData.h"
#include "Parser/ParserList.h"
#include "Parser/FileParser/ISOParser.h"

Parser::FileParser::ISOParser::ISOParser()
{
}

Parser::FileParser::ISOParser::~ISOParser()
{
}

Int32 Parser::FileParser::ISOParser::GetName()
{
	return *(Int32*)"ISOP";
}

void Parser::FileParser::ISOParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::ISOParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::SectorData)
	{
		selector->AddFilter(CSTR("*.iso"), CSTR("ISO9660 Image File"));
		selector->AddFilter(CSTR("*.img"), CSTR("Disk Image File"));
	}
}

IO::ParserType Parser::FileParser::ISOParser::GetParserType()
{
	return IO::ParserType::SectorData;
}

IO::ParsedObject *Parser::FileParser::ISOParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt8 buff[32];
	UInt64 fileSize = fd->GetDataSize();

	IO::FileSectorData *sectorData = 0;
	if (ReadMInt32(&hdr[0]) == 0x00ffffff && ReadMUInt32(&hdr[4]) == 0xffffffff && ReadMUInt32(&hdr[8]) == 0xffffff00 && fileSize >= 75264 && (fileSize % 2352) == 0)
	{
		fd->GetRealData(37632, 32, BYTEARR(buff));
		if (ReadMInt32(&buff[0]) == 0x00ffffff && ReadMUInt32(&buff[4]) == 0xffffffff && ReadMUInt32(&buff[8]) == 0xffffff00 && ReadMInt32(&buff[16]) == 0x01434430 && ReadMInt32(&buff[20]) == 0x30310100)
		{
			NEW_CLASS(sectorData, IO::FileSectorData(fd, 0, fileSize, 2352));
		}
	}

	if (sectorData == 0)// && (fileSize & 2047) == 0)
	{
		fd->GetRealData(32768, 32, BYTEARR(buff));
		if (ReadMInt32(&buff[0]) == 0x01434430 && ReadMInt32(&buff[4]) == 0x30310100)
		{
			NEW_CLASS(sectorData, IO::FileSectorData(fd, 0, fileSize, 2048));
		}
	}

	if (sectorData && targetType != IO::ParserType::SectorData)
	{
		IO::ParserType pt;
		IO::ParsedObject *pobj = this->parsers->ParseObject(sectorData, &pt);
		if (pobj)
		{
			DEL_CLASS(sectorData);
			return pobj;
		}
	}
	return sectorData;
}
