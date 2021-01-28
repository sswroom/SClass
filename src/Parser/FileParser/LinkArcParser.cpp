#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/LinkArcParser.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Parser::FileParser::LinkArcParser::LinkArcParser()
{
}

Parser::FileParser::LinkArcParser::~LinkArcParser()
{
}

Int32 Parser::FileParser::LinkArcParser::GetName()
{
	return *(Int32*)"LARC";
}

void Parser::FileParser::LinkArcParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.arc", (const UTF8Char*)"ARC File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::LinkArcParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::LinkArcParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdrBuff[32];
	Int64 currOfst;
	UInt8 nameSize;
	UInt8 recBuff[256];
	Int64 fileSize;

	UInt32 recSize;

	Int16 fnameSize;
	UTF8Char fileName[256];
	Data::DateTime dt;

	if (!Text::StrEndsWithICase(fd->GetFullName(), (const UTF8Char*)".ARC"))
	{
		return 0;
	}
	if (fd->GetRealData(0, 32, hdrBuff) != 32)
		return 0;
	if (ReadInt32(&hdrBuff[0]) != 0x4b4e494c || ReadInt16(&hdrBuff[4]) != 0x36)
		return 0;
	nameSize = hdrBuff[7];
	if (nameSize > Text::StrCharCnt(fd->GetShortName()) - 4)
		return 0;
	currOfst = 8 + nameSize;
	fileSize = fd->GetDataSize();

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	while (currOfst < fileSize)
	{
		fd->GetRealData(currOfst, 256, recBuff);
		recSize = ReadUInt32(&recBuff[0]);
		fnameSize = ReadInt16(&recBuff[13]);
		if (recSize == 0 && currOfst == fileSize - 4)
			break;
		if (recSize <= 16 || recSize + currOfst > fileSize)
		{
			DEL_CLASS(pf);
			return 0;
		}
		if (fnameSize <= 0 || fnameSize >= 256 || fnameSize & 1)
		{
			DEL_CLASS(pf);
			return 0;
		}
		dt.SetValue(ReadUInt16(&recBuff[6]), recBuff[8], recBuff[9], recBuff[10], recBuff[11], recBuff[12], 0, 36);
		Text::StrUTF16_UTF8(fileName, (const UTF16Char*)&recBuff[15], fnameSize >> 1);
		fileName[fnameSize >> 1] = 0;
		pf->AddData(fd, currOfst + 15 + fnameSize, recSize - fnameSize - 15, fileName, dt.ToTicks());

		currOfst += recSize;
	}

	return pf;
}
