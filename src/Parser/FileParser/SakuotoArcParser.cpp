#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/SakuotoArcParser.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Parser::FileParser::SakuotoArcParser::SakuotoArcParser()
{
}

Parser::FileParser::SakuotoArcParser::~SakuotoArcParser()
{
}

Int32 Parser::FileParser::SakuotoArcParser::GetName()
{
	return *(Int32*)"SARC";
}

void Parser::FileParser::SakuotoArcParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.arc", (const UTF8Char*)"ARC Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::SakuotoArcParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::SakuotoArcParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdrBuff[16];
	UInt8 *recBuff;
	UInt64 dataOfst;
	UInt32 recCnt;
	UInt32 recSize;
	UInt32 i;
	UInt32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF16Char *fileName;

	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (ReadInt32(&hdrBuff[12]) != 0)
		return 0;
	recCnt = ReadUInt32(&hdrBuff[0]);
	recSize = ReadUInt32(&hdrBuff[4]);
	if (recCnt == 0 || recCnt >= 65536)
		return 0;
	if (recSize < recCnt * 10 || recSize >= 1048576)
	{
		return 0;
	}

	recBuff = MemAlloc(UInt8, recSize);
	dataOfst = recSize + 8;
	if (fd->GetRealData(8, recSize, recBuff) != recSize)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 0;
	while (i < recSize)
	{
		fileSize = ReadUInt32(&recBuff[i]);
		fileOfst = ReadUInt32(&recBuff[i + 4]);
		if (fileOfst != nextOfst)
		{
			MemFree(recBuff);
			DEL_CLASS(pf);
			return 0;
		}
		fileName = (UTF16Char*)&recBuff[i + 8];
		while (*fileName++);
		const UTF8Char *u8ptr = Text::StrToUTF8New((UTF16Char*)&recBuff[i + 8]);
		pf->AddData(fd, dataOfst + fileOfst, fileSize, u8ptr, 0);
		Text::StrDelNew(u8ptr);

		nextOfst = fileOfst + fileSize;
		i = (UInt32)(((UInt8*)fileName) - recBuff);
		j++;
	}

	MemFree(recBuff);
	return pf;
}
