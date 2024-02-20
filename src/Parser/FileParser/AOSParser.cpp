#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/AOSParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::AOSParser::AOSParser()
{
}

Parser::FileParser::AOSParser::~AOSParser()
{
}

Int32 Parser::FileParser::AOSParser::GetName()
{
	//Found at HUNTING BLUE
	return *(Int32*)"AOSP";
}

void Parser::FileParser::AOSParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.aos"), CSTR("AOS (HUNTING BLUE) Package File"));
	}
}

IO::ParserType Parser::FileParser::AOSParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::AOSParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt8 hdrBuff[273];
	UInt32 dataOfst;
	UInt32 recSize;
	UInt32 recCnt;
	UInt32 i;
	Int32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	UTF8Char *sptr;

	if (!fd->GetFullName()->EndsWith(UTF8STRC(".AOS")))
	{
		return 0;
	}
	if (ReadInt32(&hdr[0]) != 0)
		return 0;
	dataOfst = ReadUInt32(&hdr[4]);
	recSize = ReadUInt32(&hdr[8]);
	if (recSize % 40 != 0 || dataOfst > fd->GetDataSize())
		return 0;
	if (dataOfst - recSize != 273)
		return 0;
	if (fd->GetRealData(0, 273, BYTEARR(hdrBuff)) != 273)
		return 0;
	Text::Encoding enc(932);
	sptr = enc.UTF8FromBytes(fileName, &hdrBuff[12], 255, 0);
	if (!fd->GetFullName()->EndsWith(fileName, (UOSInt)(sptr - fileName)))
	{
		return 0;
	}
	recCnt = recSize / 40;
	if (recCnt == 0)
	{
		return 0;
	}

	Data::ByteBuffer recBuff(recSize);
	if (fd->GetRealData(273, recSize, recBuff) != recSize)
	{
		return 0;
	}

	IO::VirtualPackageFile *pf;
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 32]);
		fileSize = ReadUInt32(&recBuff[j + 36]);
		if (fileOfst != nextOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], 32, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, IO::PackFileItem::HeaderType::No, CSTRP(fileName, sptr), 0, 0, 0, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += 40;
	}
	return pf;
}
