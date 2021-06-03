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

void Parser::FileParser::ISOParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_SECTOR_DATA)
	{
		selector->AddFilter((const UTF8Char*)"*.iso", (const UTF8Char*)"ISO9660 Image File");
		selector->AddFilter((const UTF8Char*)"*.img", (const UTF8Char*)"Disk Image File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::ISOParser::GetParserType()
{
	return IO::ParsedObject::PT_SECTOR_DATA;
}

IO::ParsedObject *Parser::FileParser::ISOParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[32];
	UInt64 fileSize = fd->GetDataSize();
	fd->GetRealData(0, 32, buff);

	IO::FileSectorData *sectorData = 0;
	if (ReadMInt32(&buff[0]) == 0x00ffffff && ReadMUInt32(&buff[4]) == 0xffffffff && ReadMUInt32(&buff[8]) == 0xffffff00 && fileSize >= 75264 && (fileSize % 2352) == 0)
	{
		fd->GetRealData(37632, 32, buff);
		if (ReadMInt32(&buff[0]) == 0x00ffffff && ReadMUInt32(&buff[4]) == 0xffffffff && ReadMUInt32(&buff[8]) == 0xffffff00 && ReadMInt32(&buff[16]) == 0x01434430 && ReadMInt32(&buff[20]) == 0x30310100)
		{
			NEW_CLASS(sectorData, IO::FileSectorData(fd, 0, fileSize, 2352));
		}
	}

	if (sectorData == 0)// && (fileSize & 2047) == 0)
	{
		fd->GetRealData(32768, 32, buff);
		if (ReadMInt32(&buff[0]) == 0x01434430 && ReadMInt32(&buff[4]) == 0x30310100)
		{
			NEW_CLASS(sectorData, IO::FileSectorData(fd, 0, fileSize, 2048));
		}
	}

	if (sectorData && targetType != IO::ParsedObject::PT_SECTOR_DATA)
	{
		IO::ParsedObject::ParserType pt;
		IO::ParsedObject *pobj = this->parsers->ParseObject(sectorData, &pt);
		if (pobj)
		{
			DEL_CLASS(sectorData);
			return pobj;
		}
	}
	return sectorData;
}
