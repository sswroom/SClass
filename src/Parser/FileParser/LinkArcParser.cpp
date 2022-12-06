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

void Parser::FileParser::LinkArcParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.arc"), CSTR("ARC File"));
	}
}

IO::ParserType Parser::FileParser::LinkArcParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::LinkArcParser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt64 currOfst;
	UInt8 nameSize;
	UInt8 recBuff[256];
	UInt64 fileSize;

	UInt32 recSize;

	UInt16 fnameSize;
	UTF8Char fileName[256];
	Data::DateTime dt;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".ARC")))
	{
		return 0;
	}
	if (ReadInt32(&hdr[0]) != 0x4b4e494c || ReadInt16(&hdr[4]) != 0x36)
		return 0;
	nameSize = hdr[7];
	if (nameSize > fd->GetShortName().leng - 4)
		return 0;
	currOfst = 8 + (UOSInt)nameSize;
	fileSize = fd->GetDataSize();

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	while (currOfst < fileSize)
	{
		fd->GetRealData(currOfst, 256, recBuff);
		recSize = ReadUInt32(&recBuff[0]);
		fnameSize = ReadUInt16(&recBuff[13]);
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
		Text::StrUTF16_UTF8C(fileName, (const UTF16Char*)&recBuff[15], (UOSInt)fnameSize >> 1);
		fileName[fnameSize >> 1] = 0;
		pf->AddData(fd, currOfst + 15 + fnameSize, recSize - fnameSize - 15, {fileName, (UOSInt)fnameSize >> 1}, Data::Timestamp(dt.ToTicks(), 0));

		currOfst += recSize;
	}

	return pf;
}
