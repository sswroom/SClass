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

void Parser::FileParser::CDXAParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::SectorData)
	{
		selector->AddFilter(CSTR("*.dat"), CSTR("DAT (CDXA/VCD) File"));
	}
}

IO::ParserType Parser::FileParser::CDXAParser::GetParserType()
{
	return IO::ParserType::SectorData;
}

IO::ParsedObject *Parser::FileParser::CDXAParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt32 fileSize;
	UInt64 currPos;
	if (ReadNInt32(&hdr[0]) != *(Int32*)"RIFF")
		return 0;
	if (ReadNInt32(&hdr[8]) != *(Int32*)"CDXA")
		return 0;
	fileSize = ReadUInt32(&hdr[4]) + 8;

	UInt8 riffBuff[12];
	IO::CDXAData *cdData = 0;
	UInt8 *fmt = 0;
	currPos = 12;
	while (currPos < fileSize)
	{
		fd->GetRealData(currPos, 8, BYTEARR(riffBuff));
		
		if (ReadNInt32(&riffBuff[0]) == *(Int32*)"fmt ")
		{
/*			if (fmt)
				MemFree(fmt);
			fmt = MemAlloc(UInt8, buff[1]);
			fd->GetRealData(currPos + 8, buff[1], fmt);*/
		}
		else if (ReadNInt32(&riffBuff[0]) == *(Int32*)"data")
		{
			UInt32 leng = ReadUInt32(&riffBuff[4]);
			if ((leng % 2352) == 0)
			{
				NEW_CLASS(cdData, IO::CDXAData(fd, currPos + 8, leng));
				break;
			}
			//////////////////////////
		}
		else if (ReadNUInt32(&riffBuff[0]) & 0x80808080)
		{
			if (fmt)
				MemFree(fmt);
			return 0;
		}

		currPos += ReadUInt32(&riffBuff[4]) + 8;
	}
	if (fmt)
		MemFree(fmt);
	return cdData;
}
