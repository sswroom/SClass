#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/ZWEIParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::ZWEIParser::ZWEIParser()
{
}

Parser::FileParser::ZWEIParser::~ZWEIParser()
{
}

Int32 Parser::FileParser::ZWEIParser::GetName()
{
	//Zwei!!
	return *(Int32*)"ZWEI";
}

void Parser::FileParser::ZWEIParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		//selector->AddFilter(L"*.zip", L"ZIP File");
	}
}

IO::ParserType Parser::FileParser::ZWEIParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::ZWEIParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt32 extCnt;
	UInt32 extOfst;
	UInt32 fileOfst;
	UOSInt buffOfst;
	UOSInt recOfst;
	UInt32 j;
	UInt32 i;
	UTF8Char name[14];
	UTF8Char *sptr;

	if (ReadUInt32(&hdr[0]) != 0xBC614E)
	{
		return 0;
	}

	extCnt = ReadUInt32(&hdr[4]);
	if (extCnt <= 0 || extCnt > 64)
	{
		return 0;
	}
	extOfst = 12 * extCnt + 8;
	Data::ByteBuffer extHdrs(12 * extCnt);
	fd->GetRealData(8, 12 * extCnt, extHdrs);
	UInt32 recCnt;
	UInt32 fileSize;

	IO::VirtualPackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));

	fileOfst = extOfst;
	i = 0;
	buffOfst = 0;
	while (i < extCnt)
	{
		fileOfst += ReadUInt32(&extHdrs[buffOfst + 8]) << 4;
		i++;
		buffOfst += 12;
	}

	i = 0;
	buffOfst = 0;
	while (i < extCnt)
	{

		if (ReadUInt32(&extHdrs[buffOfst + 4]) != extOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}

		recCnt = ReadUInt32(&extHdrs[buffOfst + 8]);
		if (recCnt <= 0 || recCnt > 65536)
		{
			DEL_CLASS(pf);
			return 0;
		}
		Data::ByteBuffer recHdrs(recCnt << 4);
		fd->GetRealData(extOfst, recCnt << 4, recHdrs);

		j = 0;
		recOfst = 0;
		while (j < recCnt)
		{
			if (ReadUInt32(&recHdrs[recOfst + 12]) != fileOfst)
			{
				DEL_CLASS(pf);
				return 0;
			}
			
			sptr = Text::StrConcatC(name, &recHdrs[recOfst], 8);
			*sptr++ = '.';
			sptr = Text::StrConcatC(sptr, &extHdrs[buffOfst], 4);
			
			fileSize = ReadUInt32(&recHdrs[recOfst + 8]);
			pf->AddData(fd, fileOfst, fileSize, IO::PackFileItem::HeaderType::No, CSTRP(name, sptr), 0, 0, 0, 0);
			fileOfst += fileSize;

			j++;
			recOfst += 16;
		}
		extOfst += recCnt << 4;

		i++;
		buffOfst += 12;
	}
	return pf;
}
