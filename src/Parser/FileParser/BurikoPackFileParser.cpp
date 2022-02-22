#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/BurikoPackFileParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::BurikoPackFileParser::BurikoPackFileParser()
{
}

Parser::FileParser::BurikoPackFileParser::~BurikoPackFileParser()
{
}

Int32 Parser::FileParser::BurikoPackFileParser::GetName()
{
	return *(Int32*)"BPAC";
}

void Parser::FileParser::BurikoPackFileParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.pac"), CSTR("PAC Package File"));
	}
}

IO::ParserType Parser::FileParser::BurikoPackFileParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::BurikoPackFileParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdrBuff[16];
	UInt32 dataOfst;
	UInt32 recCnt;
	UInt8 *recBuff;
	UInt32 i;
	Int32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	UTF8Char *sptr;
	Text::Encoding enc(932);

	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (ReadInt32(&hdrBuff[0]) != 0x6b636150 || ReadInt32(&hdrBuff[4]) != 0x656c6946 || ReadInt32(&hdrBuff[8]) != 0x20202020)
		return 0;
	recCnt = ReadUInt32(&hdrBuff[12]);
	if (recCnt == 0 || recCnt * 32 + 16 > fd->GetDataSize())
		return 0;
	dataOfst = recCnt * 32 + 16;
	recBuff = MemAlloc(UInt8, recCnt * 32);
	if (fd->GetRealData(16, recCnt * 32, recBuff) != recCnt * 32)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 16]);
		fileSize = ReadUInt32(&recBuff[j + 20]);
		if (fileOfst != nextOfst)
		{
			MemFree(recBuff);
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], 16, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, CSTRP(fileName, sptr), 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += 32;
	}

	MemFree(recBuff);
	return pf;
}
