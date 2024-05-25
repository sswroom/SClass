#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StreamData.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/MMSParser.h"
#include "Text/Encoding.h"

Parser::FileParser::MMSParser::MMSParser()
{
}

Parser::FileParser::MMSParser::~MMSParser()
{
}

Int32 Parser::FileParser::MMSParser::GetName()
{
	return *(Int32*)"MMSP";
}

void Parser::FileParser::MMSParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.mms"), CSTR("MMS File"));
	}
}

IO::ParserType Parser::FileParser::MMSParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::MMSParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	IO::VirtualPackageFile *pf;
	UInt32 fileCnt;
	UOSInt currOfst;
	UInt8 buff[256];
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	const UInt8 *ptr;

	if (hdr[0] != 0x8c || hdr[1] != 0x80)
		return 0;

	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	ptr = &hdr[2];
	while (*ptr & 0x80)
	{
		if (*ptr == 0x84)
		{
			ptr++;
			ptr++;
			ptr += *ptr;
			ptr++;
			break;
		}
		else if (*ptr == 0x89)
		{
			ptr++;
			ptr += *ptr;
			ptr++;
		}
		else
		{
			ptr++;
			if (*ptr & 0x80)
			{
				ptr++;
			}
			else
			{
				while (*ptr++);
			}
		}
	}

	fileCnt = *ptr++;
	currOfst = (UOSInt)(ptr - hdr.Ptr());

	while (fileCnt-- > 0)
	{
		UInt32 hdrSize;
		UInt32 dataSize;
		fd->GetRealData(currOfst, 4, BYTEARR(buff));
		hdrSize = buff[0];
		if (buff[1] & 0x80)
		{
			dataSize = (UInt32)((buff[1] & 0x7f) << 7) | (buff[2] & 0x7f);
			currOfst += 3;
		}
		else
		{
			dataSize = buff[1];
			currOfst += 2;
		}
		fd->GetRealData(currOfst, hdrSize, BYTEARR(buff));
		sbuff[0] = (UTF8Char)(0x41 + fileCnt);
		sbuff[1] = 0;
		sptr = &sbuff[1];
		ptr = buff;
		if (*ptr < hdrSize)
		{
			ptr++;
			if (*ptr & 0x80)
			{
				ptr++;
			}
			else
			{
				while (*ptr++);
			}
			if (*ptr == 0x81)
			{
				ptr += 2;
			}
			if (*ptr == 0x85)
			{
				ptr++;
				sptr = Text::StrConcat(sbuff, ptr);
			}
		}
		currOfst += hdrSize;
		pf->AddData(fd, currOfst, dataSize, IO::PackFileItem::HeaderType::No, CSTRP(sbuff, sptr), 0, 0, 0, 0);
		currOfst += dataSize;
	}
	return pf;
}
