#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/NOAParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::NOAParser::NOAParser()
{
}

Parser::FileParser::NOAParser::~NOAParser()
{
}

Int32 Parser::FileParser::NOAParser::GetName()
{
	return *(Int32*)"NOAP";
}

void Parser::FileParser::NOAParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.noa"), CSTR("NOA Package File"));
	}
}

IO::ParserType Parser::FileParser::NOAParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::NOAParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt8 tagBuff[16];
	UInt32 tagSize;
	UInt32 dataOfst;

	UInt32 recCnt;
	UInt32 fnameSize;
	UInt32 i;
	UInt32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	Data::DateTime dt;
	UTF8Char fileName[256];
	UnsafeArray<UTF8Char> sptr;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".NOA")))
	{
		return nullptr;
	}
	if (ReadInt32(&hdr[0]) != 0x69746e45 || ReadInt32(&hdr[4]) != 0x1a73)
		return nullptr;

	dataOfst = 64;
	if (ReadInt32(&hdr[64]) != 0x45726944 || ReadInt32(&hdr[68]) != 0x7972746e)
	{
		return nullptr;
	}
	tagSize = ReadUInt32(&hdr[72]);
	fd->GetRealData(64 + tagSize + 16, 16, BYTEARR(tagBuff));
	if (ReadInt32(&tagBuff[0]) != 0x656c6966 || ReadInt32(&tagBuff[4]) != 0x61746164)
	{
		return nullptr;
	}
	Data::ByteBuffer recBuff(tagSize);
	if (fd->GetRealData(64 + 16, tagSize, recBuff) != tagSize)
	{
		return nullptr;
	}

	IO::VirtualPackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
	j = 4;
	i = 0;
	recCnt = ReadUInt32(&recBuff[0]);
	while (i < recCnt)
	{
		fileSize = ReadUInt32(&recBuff[j]);
		fileOfst = ReadUInt32(&recBuff[j + 16]);
		fnameSize = ReadUInt32(&recBuff[j + 36]);
		dt.SetValue(ReadUInt16(&recBuff[j + 30]), recBuff[j + 29], recBuff[j + 28], recBuff[j + 26], recBuff[j + 25], recBuff[j + 24], 0, 36);
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j + 40], fnameSize, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, IO::PackFileItem::HeaderType::No, CSTRP(fileName, sptr), Data::Timestamp(dt.ToInstant(), 0), nullptr, nullptr, 0);

		i++;
		j += 40 + fnameSize;
	}
	return pf;
}
