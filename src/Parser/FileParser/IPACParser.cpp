#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/IPACParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::IPACParser::IPACParser()
{
}

Parser::FileParser::IPACParser::~IPACParser()
{
}

Int32 Parser::FileParser::IPACParser::GetName()
{
	return *(Int32*)"IPAC";
}

void Parser::FileParser::IPACParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
	}
}

IO::ParserType Parser::FileParser::IPACParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::IPACParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt32 recCnt;
	UInt32 startOfst;
	UInt32 currOfst;
	UInt32 currSize;
	UInt32 hdrOfst;
	UInt8 rec[44];
	UTF8Char name[33];
	UnsafeArray<UTF8Char> sptr;

	if (ReadUInt32(&hdr[0]) != 0x43415049)
	{
		return 0;
	}

	recCnt = ReadUInt32(&hdr[4]);
	hdrOfst = 8;
	currOfst = recCnt * 44 + 8;

	Text::Encoding enc(932);
	IO::VirtualPackageFile *pf;
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	UInt32 i = 0;

	while (i < recCnt)
	{
		fd->GetRealData(hdrOfst, 44, BYTEARR(rec));
		startOfst = ReadUInt32(&rec[36]);
		if (startOfst != currOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(name, rec, 32, 0);
		currSize = ReadUInt32(&rec[40]);

		pf->AddData(fd, currOfst, currSize, IO::PackFileItem::HeaderType::No, CSTRP(name, sptr), 0, 0, 0, 0);
		currOfst += currSize;
		hdrOfst += 44;
		i++;
	}

	return pf;
}
