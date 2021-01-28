#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/ZWEIParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "IO/PackageFile.h"

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

void Parser::FileParser::ZWEIParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		//selector->AddFilter(L"*.zip", L"ZIP File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::ZWEIParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::ZWEIParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	Int32 hdr[2];
	Int32 extCnt;
	UInt32 extOfst;
	UInt32 fileOfst;
	OSInt buffOfst;
	OSInt recOfst;
	Int32 j;
	Int32 i;
	Text::Encoding enc(932);
	UTF8Char name[14];
	UTF8Char *sptr;

	fd->GetRealData(0, 8, (UInt8*)hdr);
	if (hdr[0] != 0xBC614E)
	{
		return 0;
	}

	extCnt = hdr[1];
	if (extCnt <= 0 || extCnt > 64)
	{
		return 0;
	}
	extOfst = 12 * extCnt + 8;
	UInt8 *extHdrs = MemAlloc(UInt8, 12 * extCnt);
	fd->GetRealData(8, 12 * extCnt, extHdrs);
	UInt8 *recHdrs;
	Int32 recCnt;
	UInt32 fileSize;

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));

	fileOfst = extOfst;
	i = 0;
	buffOfst = 0;
	while (i < extCnt)
	{
		fileOfst += (*(UInt32*)&extHdrs[buffOfst + 8]) << 4;
		i++;
		buffOfst += 12;
	}

	i = 0;
	buffOfst = 0;
	while (i < extCnt)
	{

		if (*(UInt32*)&extHdrs[buffOfst + 4] != extOfst)
		{
			MemFree(extHdrs);
			DEL_CLASS(pf);
			return 0;
		}

		recCnt = *(Int32*)&extHdrs[buffOfst + 8];
		if (recCnt <= 0 || recCnt > 65536)
		{
			MemFree(extHdrs);
			DEL_CLASS(pf);
			return 0;
		}
		recHdrs = MemAlloc(UInt8, recCnt << 4);
		fd->GetRealData(extOfst, recCnt << 4, recHdrs);

		j = 0;
		recOfst = 0;
		while (j < recCnt)
		{
			if (*(UInt32*)&recHdrs[recOfst + 12] != fileOfst)
			{
				MemFree(extHdrs);
				MemFree(recHdrs);
				DEL_CLASS(pf);
				return 0;
			}
			
			sptr = Text::StrConcatC(name, &recHdrs[recOfst], 8);
			*sptr++ = '.';
			sptr = Text::StrConcatC(sptr, &extHdrs[buffOfst], 4);
			
			fileSize = *(UInt32*)&recHdrs[recOfst + 8];
			pf->AddData(fd, fileOfst, fileSize, name, 0);
			fileOfst += fileSize;

			j++;
			recOfst += 16;
		}
		MemFree(recHdrs);
		extOfst += recCnt << 4;

		i++;
		buffOfst += 12;
	}
	MemFree(extHdrs);
	return pf;
}
