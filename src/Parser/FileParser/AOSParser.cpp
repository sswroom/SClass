#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/AOSParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::AOSParser::AOSParser()
{
}

Parser::FileParser::AOSParser::~AOSParser()
{
}

Int32 Parser::FileParser::AOSParser::GetName()
{
	//Found at HUNTING BLUE
	return *(Int32*)"AOSP";
}

void Parser::FileParser::AOSParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.aos"), CSTR("AOS (HUNTING BLUE) Package File"));
	}
}

IO::ParserType Parser::FileParser::AOSParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::AOSParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdrBuff[273];
	UInt32 dataOfst;
	UInt32 recSize;
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

	if (!fd->GetFullName()->EndsWith(UTF8STRC(".AOS")))
	{
		return 0;
	}
	if (fd->GetRealData(0, 273, hdrBuff) != 273)
		return 0;
	if (ReadInt32(&hdrBuff[0]) != 0)
		return 0;
	dataOfst = ReadUInt32(&hdrBuff[4]);
	recSize = ReadUInt32(&hdrBuff[8]);
	if (recSize % 40 != 0 || dataOfst > fd->GetDataSize())
		return 0;
	if (dataOfst - recSize != 273)
		return 0;
	sptr = enc.UTF8FromBytes(fileName, &hdrBuff[12], 255, 0);
	if (!fd->GetFullName()->EndsWith(fileName, (UOSInt)(sptr - fileName)))
	{
		return 0;
	}
	recCnt = recSize / 40;
	if (recCnt == 0)
	{
		return 0;
	}

	recBuff = MemAlloc(UInt8, recSize);
	if (fd->GetRealData(273, recSize, recBuff) != recSize)
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
		fileOfst = ReadUInt32(&recBuff[j + 32]);
		fileSize = ReadUInt32(&recBuff[j + 36]);
		if (fileOfst != nextOfst)
		{
			MemFree(recBuff);
			DEL_CLASS(pf);
			return 0;
		}
		enc.UTF8FromBytes(fileName, &recBuff[j], 32, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, fileName, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += 40;
	}

	MemFree(recBuff);
	return pf;
}
