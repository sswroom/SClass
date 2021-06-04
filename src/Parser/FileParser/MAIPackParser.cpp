#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/MAIPackParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::MAIPackParser::MAIPackParser()
{
}

Parser::FileParser::MAIPackParser::~MAIPackParser()
{
}

Int32 Parser::FileParser::MAIPackParser::GetName()
{
	return *(Int32*)"MAIP";
}

void Parser::FileParser::MAIPackParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.mai", (const UTF8Char*)"MAI Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::MAIPackParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::MAIPackParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdrbuff[16];
	UInt8 recbuff[24];
	UInt32 hdrEnd;
	UInt32 hdrOfst;
	UInt32 fileOfst;
	UInt32 thisOfst;
	UInt32 thisSize;

	UTF8Char name[17];
	Text::Encoding enc(932);

	fd->GetRealData(0, 16, hdrbuff);
	if (*(Int32*)&hdrbuff[0] != 0x0A49414D || *(UInt32*)&hdrbuff[4] != fd->GetDataSize() || *(UInt32*)&hdrbuff[12] != 0x100)
	{
		return 0;
	}

	hdrEnd = ReadUInt32(&hdrbuff[8]) * 24 + 16;
	fileOfst = hdrEnd;
	hdrOfst = 16;
	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));

	while (hdrOfst < hdrEnd)
	{
		fd->GetRealData(hdrOfst, 24, recbuff);
		thisOfst = ReadUInt32(&recbuff[16]);
		thisSize = ReadUInt32(&recbuff[20]);
		if (thisOfst != fileOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}
		enc.UTF8FromBytes(name, recbuff, 16, 0);
		pf->AddData(fd, thisOfst, thisSize, name, 0);

		fileOfst += thisSize;
		hdrOfst += 24;
	}
	return pf;
}
