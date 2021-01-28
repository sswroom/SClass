#include "Stdafx.h"
#include "MyMemory.h"
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

void Parser::FileParser::IPACParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
	}
}

IO::ParsedObject::ParserType Parser::FileParser::IPACParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::IPACParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	Int32 hdr[2];
	Int32 recCnt;
	Int32 startOfst;
	Int32 currOfst;
	UInt32 currSize;
	Int32 hdrOfst;
	UInt8 rec[44];
	UTF8Char name[33];

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
	Int32 i = 0;

	while (i < recCnt)
	{
		fd->GetRealData(hdrOfst, 44, rec);
		startOfst = *(Int32*)&rec[36];
		if (startOfst != currOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}
		enc.UTF8FromBytes(name, rec, 32, 0);
		currSize = *(UInt32*)&rec[40];

		pf->AddData(fd, currOfst, (Int32)currSize, name, 0);
		currOfst += currSize;
		hdrOfst += 44;
		i++;
	}

	return pf;
}
