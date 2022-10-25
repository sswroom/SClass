#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Parser/FileParser/IPACParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "IO/PackageFile.h"

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

void Parser::FileParser::IPACParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
	}
}

IO::ParserType Parser::FileParser::IPACParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::IPACParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt32 hdr[2];
	UInt32 recCnt;
	UInt32 startOfst;
	UInt32 currOfst;
	UInt32 currSize;
	UInt32 hdrOfst;
	UInt8 rec[44];
	UTF8Char name[33];
	UTF8Char *sptr;

	fd->GetRealData(0, 8, (UInt8*)hdr);
	if (hdr[0] != 0x43415049)
	{
		return 0;
	}

	recCnt = hdr[1];
	hdrOfst = 8;
	currOfst = recCnt * 44 + 8;

	Text::Encoding enc(932);
	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	UInt32 i = 0;

	while (i < recCnt)
	{
		fd->GetRealData(hdrOfst, 44, rec);
		startOfst = ReadUInt32(&rec[36]);
		if (startOfst != currOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(name, rec, 32, 0);
		currSize = ReadUInt32(&rec[40]);

		pf->AddData(fd, currOfst, currSize, CSTRP(name, sptr), Data::Timestamp(0, 0));
		currOfst += currSize;
		hdrOfst += 44;
		i++;
	}

	return pf;
}
