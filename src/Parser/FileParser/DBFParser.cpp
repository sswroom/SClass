#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "DB/DBFFile.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/DBFParser.h"

Parser::FileParser::DBFParser::DBFParser()
{
	this->codePage = 0;
}

Parser::FileParser::DBFParser::~DBFParser()
{
}

Int32 Parser::FileParser::DBFParser::GetName()
{
	return *(Int32*)"DBFP";
}

void Parser::FileParser::DBFParser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::DBFParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_READINGDB_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.dbf", (const UTF8Char*)"DBase File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::DBFParser::GetParserType()
{
	return IO::ParsedObject::PT_READINGDB_PARSER;
}

IO::ParsedObject *Parser::FileParser::DBFParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdr[32];
	UInt8 hdr2;
	fd->GetRealData(0, 32, hdr);

	if (((*(UInt16*)&hdr[8]) & 31) != 1)
	{
		return 0;
	}
	
	if (fd->GetRealData((UOSInt)ReadUInt16(&hdr[8]) - 1, 1, &hdr2) != 1)
	{
		return 0;
	}
	if (hdr2 != 0xd)
	{
		return 0;
	}
	UInt16 leng = *(UInt16*)&hdr[10];
	UInt32 nRecs = *(UInt32*)&hdr[4];
	if (leng > 4096)
	{
		return 0;
	}
	if (nRecs == 0)
	{
		return 0;
	}
	if (nRecs * (UInt64)leng + *(UInt16*)&hdr[8] > fd->GetDataSize())
	{
		return 0;
	}

	DB::DBFFile *pf;
	NEW_CLASS(pf, DB::DBFFile(fd, this->codePage));

	return pf;
}
