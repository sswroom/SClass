#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/X13Parser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "IO/PackageFile.h"

Parser::FileParser::X13Parser::X13Parser()
{
}

Parser::FileParser::X13Parser::~X13Parser()
{
}

Int32 Parser::FileParser::X13Parser::GetName()
{
	//From Hooligans
	return *(Int32*)"X13P";
}

void Parser::FileParser::X13Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.x13", (const UTF8Char*)"X13 (Hooligans) Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::X13Parser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::X13Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	Text::Encoding enc;
	UInt32 hdr[3];
	OSInt buffOfst;
	UTF8Char name[49];

	fd->GetRealData(0, 12, (UInt8*)hdr);
	if (hdr[0] != 0x4B434150 || (hdr[1] + hdr[2]) != fd->GetDataSize() || (hdr[2] & 63) != 0)
	{
		return 0;
	}

	IO::PackageFile *pf;
	UInt8 *recHdrs = MemAlloc(UInt8, hdr[2]);
	fd->GetRealData(hdr[1], hdr[2], recHdrs);
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	buffOfst = 0;
	while (buffOfst < (OSInt)hdr[2])
	{
		enc.UTF8FromBytes(name, &recHdrs[buffOfst], 48, 0);
		pf->AddData(fd, *(UInt32*)&recHdrs[buffOfst + 56], *(UInt32*)&recHdrs[buffOfst + 60], name, 0);
		buffOfst += 64;
	}
	MemFree(recHdrs);
	return pf;
}
