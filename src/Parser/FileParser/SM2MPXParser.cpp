#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/SM2MPXParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "IO/PackageFile.h"

Parser::FileParser::SM2MPXParser::SM2MPXParser()
{
}

Parser::FileParser::SM2MPXParser::~SM2MPXParser()
{
}

Int32 Parser::FileParser::SM2MPXParser::GetName()
{
	return *(Int32*)"SM2M";
}

void Parser::FileParser::SM2MPXParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.sm2", (const UTF8Char*)"SM2 Package File");
		selector->AddFilter((const UTF8Char*)"*.mpx", (const UTF8Char*)"MPX Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::SM2MPXParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::SM2MPXParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdr[32];
	UInt8 rec[20];

	Text::Encoding enc(932);
	UTF8Char name[13];

	fd->GetRealData(0, 32, hdr);
	if (*(Int32*)&hdr[0] != 0x4D324D53 || *(Int32*)&hdr[4] != 0x30315850)
	{
		return 0;
	}

	Int32 cnt = *(Int32*)&hdr[8];
	Int32 endOfst = *(Int32*)&hdr[12];
	UInt32 hdrOfst = 32;
	UInt32 lastOfst;
	if (cnt * 20 != endOfst - 32 || *(Int32*)&hdr[28] != 32)
	{
		return 0;
	}

	UInt32 thisOfst;
	UInt32 thisSize;
	lastOfst = endOfst;
	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	while (hdrOfst < (UInt32)endOfst)
	{
		fd->GetRealData(hdrOfst, 20, rec);
		thisOfst = *(UInt32*)&rec[12];
		thisSize = *(UInt32*)&rec[16];
		if (thisOfst < lastOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}

		enc.UTF8FromBytes(name, rec, 12, 0);
		pf->AddData(fd, thisOfst, thisSize, name, 0);

		lastOfst = thisOfst + thisSize;
		hdrOfst += 20;
	}

	return pf;
}
