#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
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

void Parser::FileParser::BSAParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::BSAParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
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
	UTF8Char *sptr;

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

	IO::PackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
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
			pf->AddData(fd, fileOfst, fileSize, CSTRP(fileName, sptr), 0, 0, 0);
		}
		i++;
		j += 12;
	}
	return pf;
}
