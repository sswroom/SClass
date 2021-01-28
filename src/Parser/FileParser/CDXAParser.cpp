#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/CDXAData.h"
#include "Parser/FileParser/CDXAParser.h"

Parser::FileParser::CDXAParser::CDXAParser()
{
}

Parser::FileParser::CDXAParser::~CDXAParser()
{
}

Int32 Parser::FileParser::CDXAParser::GetName()
{
	return *(Int32*)"CDXA";
}

void Parser::FileParser::CDXAParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::CDXAParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_SECTOR_DATA)
	{
		selector->AddFilter((const UTF8Char*)"*.dat", (const UTF8Char*)"DAT (CDXA/VCD) File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::CDXAParser::GetParserType()
{
	return IO::ParsedObject::PT_SECTOR_DATA;
}

IO::ParsedObject *Parser::FileParser::CDXAParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	Int32 buff[4];
	UInt32 fileSize;
	Int64 currPos;
	fd->GetRealData(0, 12, (UInt8*)buff);
	if (buff[0] != *(Int32*)"RIFF")
		return 0;
	if (buff[2] != *(Int32*)"CDXA")
		return 0;
	fileSize = (UInt32)buff[1] + 8;

	IO::CDXAData *cdData = 0;
	UInt8 *fmt = 0;
	currPos = 12;
	while (currPos < fileSize)
	{
		fd->GetRealData(currPos, 8, (UInt8*)buff);
		
		if (buff[0] == *(Int32*)"fmt ")
		{
/*			if (fmt)
				MemFree(fmt);
			fmt = MemAlloc(UInt8, buff[1]);
			fd->GetRealData(currPos + 8, buff[1], fmt);*/
		}
		else if (buff[0] == *(Int32*)"data")
		{
			UInt32 leng = (UInt32)buff[1];
			if ((leng % 2352) == 0)
			{
				NEW_CLASS(cdData, IO::CDXAData(fd, currPos + 8, leng));
				break;
			}
			//////////////////////////
		}
		else if (buff[0] & 0x80808080)
		{
			if (fmt)
				MemFree(fmt);
			return 0;
		}

		currPos += (UInt32)buff[1] + 8;
	}
	if (fmt)
		MemFree(fmt);
	return cdData;
}
