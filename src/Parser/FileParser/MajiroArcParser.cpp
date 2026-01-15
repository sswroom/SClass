#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/MajiroArcParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::MajiroArcParser::MajiroArcParser()
{
}

Parser::FileParser::MajiroArcParser::~MajiroArcParser()
{
}

Int32 Parser::FileParser::MajiroArcParser::GetName()
{
	return *(Int32*)"MARC";
}

void Parser::FileParser::MajiroArcParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.arc"), CSTR("ARC Package File"));
	}
}

IO::ParserType Parser::FileParser::MajiroArcParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

/*typedef struct
{
	Char idChars[16];
	UInt32 fileCnt;
	UInt32 fileNameOfst;
	UInt32 fileNameEndOfst;
} ARCHeader;*/

/*typedef struct
{
	UInt8 fileCheck[8];
	UInt32 recOfst;
	UInt32 recSize;
} ARCRecord;*/

Optional<IO::ParsedObject> Parser::FileParser::MajiroArcParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	Data::ByteArray fileNamePtr;
	Data::ByteArray fileNamePtr2;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt32 i;

	if (!Text::StrEquals(&hdr[0], U8STR("MajiroArcV3.000")))
	{
		return nullptr;
	}
	UInt32 fileCnt = ReadUInt32(&hdr[16]);
	UInt32 fileNameOfst = ReadUInt32(&hdr[20]);
	UInt32 fileNameEndOfst = ReadUInt32(&hdr[24]);
	Data::ByteBuffer fileNameBuff(fileNameEndOfst - fileNameOfst);
	fd->GetRealData(fileNameOfst, fileNameEndOfst - fileNameOfst, fileNameBuff);
	Data::ByteBuffer recBuff(fileCnt * 16);
	fd->GetRealData(28, 16 * fileCnt, recBuff);

	IO::VirtualPackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	fileNamePtr = fileNameBuff;
	i = 0;
	
	while (i < fileCnt)
	{
		fileNamePtr2 = fileNamePtr;
		while (*fileNamePtr2++);
		sptr = enc.UTF8FromBytes(sbuff, fileNamePtr.Arr(), (UIntOS)(fileNamePtr2 - fileNamePtr - 1), 0);
		pf->AddData(fd, ReadUInt32(&recBuff[i * 16 + 8]), ReadUInt32(&recBuff[i * 16 + 12]), IO::PackFileItem::HeaderType::No, CSTRP(sbuff, sptr), nullptr, nullptr, nullptr, 0);

		fileNamePtr = fileNamePtr2;
		i++;
	}
	return pf;
}
