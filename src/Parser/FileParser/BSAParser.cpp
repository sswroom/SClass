#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/BSAParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::BSAParser::BSAParser()
{
}

Parser::FileParser::BSAParser::~BSAParser()
{
}

Int32 Parser::FileParser::BSAParser::GetName()
{
	return *(Int32*)"BSAP";
}

void Parser::FileParser::BSAParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.bsa"), CSTR("BSA Package File"));
	}
}

IO::ParserType Parser::FileParser::BSAParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::BSAParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
//	UInt16 ver;
	UOSInt recCnt;
	UInt32 recOfst;

	UOSInt recSize;
	UInt32 i;
	Int32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
//	UInt32 nextOfst;
	UTF8Char fileName[512];
	UnsafeArray<UTF8Char> sptr;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".BSA")))
	{
		return 0;
	}
	if (ReadInt32(&hdr[0]) != 0x72415342 || ReadInt32(&hdr[4]) != 0x63)
		return 0;
//	ver = ReadUInt16(&hdr[8]);
	recCnt = ReadUInt16(&hdr[10]);
	recOfst = ReadUInt32(&hdr[12]);

	if (recOfst >= fd->GetDataSize() - recCnt * 12)
		return 0;

	recSize = (UOSInt)(fd->GetDataSize() - recOfst);
	Data::ByteBuffer recBuff(recSize);
	if (fd->GetRealData(recOfst, recSize, recBuff) != recSize)
	{
		return 0;
	}

	IO::VirtualPackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
	j = 0;
	i = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 4]);
		fileSize = ReadUInt32(&recBuff[j + 8]);
		if (fileSize == 0 && fileOfst == 0)
		{
		}
		else
		{
			UInt8 *buff = &recBuff[recCnt * 12 + ReadUInt32(&recBuff[j])];
			sptr = enc.UTF8FromBytes(fileName, buff, Text::StrCharCnt(buff), 0);
			pf->AddData(fd, fileOfst, fileSize, IO::PackFileItem::HeaderType::No, CSTRP(fileName, sptr), 0, 0, 0, 0);
		}
		i++;
		j += 12;
	}
	return pf;
}
