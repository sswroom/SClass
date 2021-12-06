#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/BurikoArcParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::BurikoArcParser::BurikoArcParser()
{
}

Parser::FileParser::BurikoArcParser::~BurikoArcParser()
{
}

Int32 Parser::FileParser::BurikoArcParser::GetName()
{
	return *(Int32*)"BARC";
}

void Parser::FileParser::BurikoArcParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter((const UTF8Char*)"*.arc", (const UTF8Char*)"ARC Package File");
	}
}

IO::ParserType Parser::FileParser::BurikoArcParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::BurikoArcParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdrBuff[16];
	UInt32 recCnt;
	UInt8 *recBuff;
	UInt32 i;
	Int32 j;

	Int64 dataOfst;

	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	Text::Encoding enc(932);

	if (!fd->GetFullName()->EndsWithICase((const UTF8Char*)".ARC"))
	{
		return 0;
	}
	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (ReadInt32(&hdrBuff[0]) != 0x49525542 || ReadInt32(&hdrBuff[4]) != 0x41204f4b || ReadInt32(&hdrBuff[8]) != 0x30324352)
		return 0;
	recCnt = ReadUInt32(&hdrBuff[12]);
	if (recCnt == 0 || recCnt * 128 + 16 >= fd->GetDataSize())
	{
		return 0;
	}

	recBuff = MemAlloc(UInt8, recCnt * 128);
	if (fd->GetRealData(16, recCnt * 128, recBuff) != recCnt * 128)
	{
		MemFree(recBuff);
		return 0;
	}

	dataOfst = 16 + recCnt * 128;
	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 96]);
		fileSize = ReadUInt32(&recBuff[j + 100]);
		if (fileOfst != nextOfst)
		{
			MemFree(recBuff);
			DEL_CLASS(pf);
			return 0;
		}
		enc.UTF8FromBytes(fileName, &recBuff[j], 96, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, fileName, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += 128;
	}

	MemFree(recBuff);
	return pf;
}
