#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/BSAParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::BSAParser::BSAParser()
{
}

Parser::FileParser::BSAParser::~BSAParser()
{
}

Int32 Parser::FileParser::BSAParser::GetName()
{
	return *(Int32*)"BSAP";
}

void Parser::FileParser::BSAParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter((const UTF8Char*)"*.bsa", (const UTF8Char*)"BSA Package File");
	}
}

IO::ParserType Parser::FileParser::BSAParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::BSAParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdrBuff[16];
//	UInt16 ver;
	UOSInt recCnt;
	UInt32 recOfst;

	UOSInt recSize;
	UInt8 *recBuff;
	UInt32 i;
	Int32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
//	UInt32 nextOfst;
	UTF8Char fileName[512];
	Text::Encoding enc(932);

	if (!fd->GetFullName()->EndsWithICase((const UTF8Char*)".BSA"))
	{
		return 0;
	}
	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (ReadInt32(&hdrBuff[0]) != 0x72415342 || ReadInt32(&hdrBuff[4]) != 0x63)
		return 0;
//	ver = ReadUInt16(&hdrBuff[8]);
	recCnt = ReadUInt16(&hdrBuff[10]);
	recOfst = ReadUInt32(&hdrBuff[12]);

	if (recOfst >= fd->GetDataSize() - recCnt * 12)
		return 0;

	recSize = (UOSInt)(fd->GetDataSize() - recOfst);
	recBuff = MemAlloc(UInt8, recSize);
	if (fd->GetRealData(recOfst, recSize, recBuff) != recSize)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 4]);
		fileSize = ReadUInt32(&recBuff[j + 8]);
		if (fileSize == 0 && fileOfst == 0)
		{
		}
		else
		{
			UInt8 *buff = &recBuff[recCnt * 12 + ReadUInt32(&recBuff[j])];
			enc.UTF8FromBytes(fileName, buff, Text::StrCharCnt(buff), 0);
			pf->AddData(fd, fileOfst, fileSize, fileName, 0);
		}
		i++;
		j += 12;
	}

	MemFree(recBuff);
	return pf;
}
