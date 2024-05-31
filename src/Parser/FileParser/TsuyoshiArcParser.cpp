#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
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

void Parser::FileParser::TsuyoshiArcParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.arc"), CSTR("ARC Package File"));
	}
}

IO::ParserType Parser::FileParser::TsuyoshiArcParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::TsuyoshiArcParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt32 recCnt;

	UInt8 *filePtr;
	UInt32 recOfst;
	UInt32 recSize;
	UInt32 decSize;
	UInt8 b;
	UInt32 i;
	Int32 j;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	UnsafeArray<UTF8Char> sptr;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".ARC")))
	{
		return 0;
	}
	recCnt = ReadUInt32(&hdr[0]);
	if (recCnt == 0 || recCnt >= 65536)
		return 0;
	if (fd->GetDataSize() <= recCnt * 272)
		return 0;
	
	Data::ByteBuffer recBuff(recCnt * 272);
	if (fd->GetRealData(4, recCnt * 272, recBuff) != recCnt * 272)
	{
		return 0;
	}

	IO::VirtualPackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
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
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], Text::StrCharCnt(&recBuff[j]), 0);
		if (recSize == decSize)
		{
			pf->AddData(fd, recOfst, recSize, IO::PackFileItem::HeaderType::No, CSTRP(fileName, sptr), 0, 0, 0, 0);
		}
		else
		{
			IO::PackFileItem::CompressInfo compInfo;
			compInfo.checkMethod = Crypto::Hash::HashType::Unknown;
			compInfo.compExtraSize = 0;
			compInfo.compFlags = 0;
			compInfo.compMethod = Data::Compress::Decompressor::CM_DEFLATE;
			compInfo.decSize = decSize;
			pf->AddCompData(fd, recOfst, recSize, IO::PackFileItem::HeaderType::No, &compInfo, CSTRP(fileName, sptr), 0, 0, 0, 0);
		}

		nextOfst = recOfst + recSize;
		i++;
		j += 272;
	}
	return pf;
}
