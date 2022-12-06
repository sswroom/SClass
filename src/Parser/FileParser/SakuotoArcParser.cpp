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

void Parser::FileParser::SakuotoArcParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.arc"), CSTR("ARC Package File"));
	}
}

IO::ParserType Parser::FileParser::SakuotoArcParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::SakuotoArcParser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
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

	if (ReadInt32(&hdr[12]) != 0)
		return 0;
	recCnt = ReadUInt32(&hdr[0]);
	recSize = ReadUInt32(&hdr[4]);
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
		Text::String *s = Text::String::NewNotNull((UTF16Char*)&recBuff[i + 8]);
		pf->AddData(fd, dataOfst + fileOfst, fileSize, s->ToCString(), Data::Timestamp(0));
		s->Release();

		nextOfst = fileOfst + fileSize;
		i = (UInt32)(((UInt8*)fileName) - recBuff);
		j++;
	}

	MemFree(recBuff);
	return pf;
}
