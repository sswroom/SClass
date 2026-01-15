#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/VirtualPackageFileFast.h"
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

void Parser::FileParser::BurikoPackFileParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::BurikoPackFileParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt32 dataOfst;
	UInt32 recCnt;
	UInt32 i;
	Int32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	UnsafeArray<UTF8Char> sptr;

	if (ReadInt32(&hdr[0]) != 0x6b636150 || ReadInt32(&hdr[4]) != 0x656c6946 || ReadInt32(&hdr[8]) != 0x20202020)
		return nullptr;
	recCnt = ReadUInt32(&hdr[12]);
	if (recCnt == 0 || recCnt * 32 + 16 > fd->GetDataSize())
		return nullptr;
	dataOfst = recCnt * 32 + 16;
	Data::ByteBuffer recBuff(recCnt * 32);
	if (fd->GetRealData(16, recCnt * 32, recBuff) != recCnt * 32)
	{
		return nullptr;
	}

	IO::VirtualPackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 16]);
		fileSize = ReadUInt32(&recBuff[j + 20]);
		if (fileOfst != nextOfst)
		{
			DEL_CLASS(pf);
			return nullptr;
		}
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], 16, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, IO::PackFileItem::HeaderType::No, CSTRP(fileName, sptr), 0, 0, 0, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += 32;
	}
	return pf;
}
