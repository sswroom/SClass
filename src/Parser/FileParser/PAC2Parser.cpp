#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/PAC2Parser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::PAC2Parser::PAC2Parser()
{
}

Parser::FileParser::PAC2Parser::~PAC2Parser()
{
}

Int32 Parser::FileParser::PAC2Parser::GetName()
{
	return *(Int32*)"PAC2";
}

void Parser::FileParser::PAC2Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.pac"), CSTR("PAC Package File"));
	}
}

IO::ParserType Parser::FileParser::PAC2Parser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::PAC2Parser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt32 recCnt;
	UInt32 dataOfst;
	UTF8Char fileName[256];
	UTF8Char *sptr;

	Int64 ofst;
	UInt32 i;
	UInt32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 fnameSize;
	UInt32 nextOfst;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".PAC")))
	{
		return 0;
	}
	if (ReadUInt32(&hdr[0]) != 0xad82cf82)
		return 0;
	recCnt = ReadUInt32(&hdr[4]);
	dataOfst = ReadUInt32(&hdr[8]);
	if (recCnt == 0)
	{
		return 0;
	}
	ofst = 16 + recCnt * 268;
	if (ofst & 15)
		ofst = ofst + 16 - (ofst & 15);
	if (dataOfst != ofst)
		return 0;
	Data::ByteBuffer recBuff(268 * recCnt);
	if (fd->GetRealData(16, 268 * recCnt, recBuff) != 268 * recCnt)
	{
		return 0;
	}

	IO::VirtualPackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 256]);
		fileSize = ReadUInt32(&recBuff[j + 260]);
		fnameSize = ReadUInt32(&recBuff[j + 264]);
		if (fileOfst != nextOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], fnameSize, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, IO::PackFileItem::HeaderType::No, CSTRP(fileName, sptr), 0, 0, 0, 0);

		nextOfst = fileOfst + fileSize;
		if (nextOfst & 15)
			nextOfst = nextOfst + 16 - (nextOfst & 15);
		i++;
		j += 268;
	}
	return pf;
}
