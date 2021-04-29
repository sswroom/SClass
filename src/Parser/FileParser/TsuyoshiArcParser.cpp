#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/TsuyoshiArcParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::TsuyoshiArcParser::TsuyoshiArcParser()
{
}

Parser::FileParser::TsuyoshiArcParser::~TsuyoshiArcParser()
{
}

Int32 Parser::FileParser::TsuyoshiArcParser::GetName()
{
	return *(Int32*)"TARC";
}

void Parser::FileParser::TsuyoshiArcParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.arc", (const UTF8Char*)"ARC Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::TsuyoshiArcParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::TsuyoshiArcParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt32 recCnt;
	UInt8 *recBuff;

	UInt8 *filePtr;
	UInt32 recOfst;
	UInt32 recSize;
	UInt32 decSize;
	UInt8 b;
	UInt32 i;
	Int32 j;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	Text::Encoding enc(932);

	if (!Text::StrEndsWithICase(fd->GetFullName(), (const UTF8Char*)".ARC"))
	{
		return 0;
	}
	if (fd->GetRealData(0, 4, (UInt8*)&recCnt) != 4)
		return 0;
	if (recCnt == 0 || recCnt >= 65536)
		return 0;
	if (fd->GetDataSize() <= recCnt * 272)
		return 0;
	
	recBuff = MemAlloc(UInt8, recCnt * 272);
	if (fd->GetRealData(4, recCnt * 272, recBuff) != recCnt * 272)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 4 + recCnt * 272;
	while (i < recCnt)
	{
		recSize = ReadMUInt32(&recBuff[j + 260]);
		decSize = ReadMUInt32(&recBuff[j + 264]);
		recOfst = ReadMUInt32(&recBuff[j + 268]);
		if (recOfst != nextOfst)
		{
			MemFree(recBuff);
			DEL_CLASS(pf);
			return 0;
		}
		filePtr = &recBuff[j];
		while (*filePtr++);
		filePtr--;
		b = 2;
		while (filePtr > &recBuff[j])
		{
			filePtr--;
			*filePtr = (UInt8)(*filePtr - b);
			b++;
		}
		enc.UTF8FromBytes(fileName, &recBuff[j], Text::StrCharCnt(&recBuff[j]), 0);
		if (recSize == decSize)
		{
			pf->AddData(fd, recOfst, recSize, fileName, 0);
		}
		else
		{
			IO::PackFileItem::CompressInfo compInfo;
			compInfo.checkMethod = Crypto::Hash::HT_UNKNOWN;
			compInfo.compExtraSize = 0;
			compInfo.compFlags = 0;
			compInfo.compMethod = Data::Compress::Decompressor::CM_DEFLATE;
			compInfo.decSize = decSize;
			pf->AddCompData(fd, recOfst, recSize, &compInfo, fileName, 0);
		}

		nextOfst = recOfst + recSize;
		i++;
		j += 272;
	}

	MemFree(recBuff);
	return pf;
}
