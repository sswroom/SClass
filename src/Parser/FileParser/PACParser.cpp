#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/PACParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::PACParser::PACParser()
{
}

Parser::FileParser::PACParser::~PACParser()
{
}

Int32 Parser::FileParser::PACParser::GetName()
{
	return *(Int32*)"PACP";
}

void Parser::FileParser::PACParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter((const UTF8Char*)"*.pac", (const UTF8Char*)"PAC Package File");
	}
}

IO::ParserType Parser::FileParser::PACParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::PACParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdrBuff[16];
	UInt32 recCnt;
	UInt8 *recBuff;
	UInt32 i;
	UInt32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	Text::Encoding enc(932);

	if (!Text::StrEndsWithICase(fd->GetFullName(), (const UTF8Char*)".PAC"))
	{
		return 0;
	}
	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (*(Int32*)&hdrBuff[0] != *(Int32*)"PAC ")
		return 0;
	recCnt = ReadUInt32(&hdrBuff[8]);
	if (recCnt == 0)
	{
		return 0;
	}
	if (recCnt * 40 + 2048 + 4 >= fd->GetDataSize())
		return 0;

	recBuff = MemAlloc(UInt8, 40 * recCnt);
	if (fd->GetRealData(2052, 40 * recCnt, recBuff) != 40 * recCnt)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = recCnt * 40 + 2048 + 4;
	while (i < recCnt)
	{
		fileSize = ReadUInt32(&recBuff[j + 32]);
		fileOfst = ReadUInt32(&recBuff[j + 36]);
		if (fileOfst != nextOfst)
		{
			MemFree(recBuff);
			DEL_CLASS(pf);
			return 0;
		}
		enc.UTF8FromBytes(fileName, &recBuff[j], 32, 0);
		pf->AddData(fd, fileOfst, fileSize, fileName, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += 40;
	}

	MemFree(recBuff);
	return pf;
}
