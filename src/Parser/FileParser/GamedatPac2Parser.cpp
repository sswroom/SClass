#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/GamedatPac2Parser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::GamedatPac2Parser::GamedatPac2Parser()
{
}

Parser::FileParser::GamedatPac2Parser::~GamedatPac2Parser()
{
}

Int32 Parser::FileParser::GamedatPac2Parser::GetName()
{
	return *(Int32*)"GDAT";
}

void Parser::FileParser::GamedatPac2Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.dat"), CSTR("DAT Package File"));
	}
}

IO::ParserType Parser::FileParser::GamedatPac2Parser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::GamedatPac2Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdrBuff[16];
	UInt32 recCnt;
	UInt8 *recBuff;
	UInt32 i;
	UInt32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 dataOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	Text::Encoding enc(932);

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".dat")))
	{
		return 0;
	}
	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (ReadInt32(&hdrBuff[0]) != 0x454d4147 || ReadInt32(&hdrBuff[4]) != 0x20544144 || ReadInt32(&hdrBuff[8]) != 0x32434150)
		return 0;
	recCnt = ReadUInt32(&hdrBuff[12]);
	if (recCnt == 0 || recCnt >= 65536)
		return 0;

	recBuff = MemAlloc(UInt8, recCnt * 40);
	if (fd->GetRealData(16, recCnt * 40, recBuff) != recCnt * 40)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = recCnt * 32;
	i = 0;
	dataOfst = 16 + recCnt * 40;
	nextOfst = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 0]);
		fileSize = ReadUInt32(&recBuff[j + 4]);
		if (fileOfst != nextOfst)
		{
			MemFree(recBuff);
			DEL_CLASS(pf);
			return 0;
		}
		enc.UTF8FromBytes(fileName, &recBuff[i * 32], 32, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, fileName, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += 8;
	}

	MemFree(recBuff);
	return pf;
}
