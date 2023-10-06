#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/BurikoArcParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::BurikoArcParser::BurikoArcParser()
{
}

Parser::FileParser::BurikoArcParser::~BurikoArcParser()
{
}

Int32 Parser::FileParser::BurikoArcParser::GetName()
{
	return *(Int32*)"BARC";
}

void Parser::FileParser::BurikoArcParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.arc"), CSTR("ARC Package File"));
	}
}

IO::ParserType Parser::FileParser::BurikoArcParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::BurikoArcParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt32 recCnt;
	UInt32 i;
	Int32 j;

	Int64 dataOfst;

	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	UTF8Char *sptr;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".ARC")))
	{
		return 0;
	}
	if (ReadInt32(&hdr[0]) != 0x49525542 || ReadInt32(&hdr[4]) != 0x41204f4b || ReadInt32(&hdr[8]) != 0x30324352)
		return 0;
	recCnt = ReadUInt32(&hdr[12]);
	if (recCnt == 0 || recCnt * 128 + 16 >= fd->GetDataSize())
	{
		return 0;
	}

	Data::ByteBuffer recBuff(recCnt * 128);
	if (fd->GetRealData(16, recCnt * 128, recBuff) != recCnt * 128)
	{
		return 0;
	}

	dataOfst = 16 + recCnt * 128;
	IO::PackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 96]);
		fileSize = ReadUInt32(&recBuff[j + 100]);
		if (fileOfst != nextOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], 96, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, CSTRP(fileName, sptr), 0, 0, 0, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += 128;
	}
	return pf;
}
