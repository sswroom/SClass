#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Data/DateTime.h"
#include "IO/VirtualPackageFileFast.h"
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

void Parser::FileParser::LinkArcParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::LinkArcParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
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
		return nullptr;
	}
	if (ReadInt32(&hdr[0]) != 0x4b4e494c || ReadInt16(&hdr[4]) != 0x36)
		return nullptr;
	nameSize = hdr[7];
	if (nameSize > fd->GetShortName().leng - 4)
		return nullptr;
	currOfst = 8 + (UIntOS)nameSize;
	fileSize = fd->GetDataSize();

	IO::VirtualPackageFile *pf;
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
	while (currOfst < fileSize)
	{
		fd->GetRealData(currOfst, 256, BYTEARR(recBuff));
		recSize = ReadUInt32(&recBuff[0]);
		fnameSize = ReadUInt16(&recBuff[13]);
		if (recSize == 0 && currOfst == fileSize - 4)
			break;
		if (recSize <= 16 || recSize + currOfst > fileSize)
		{
			DEL_CLASS(pf);
			return nullptr;
		}
		if (fnameSize <= 0 || fnameSize >= 256 || fnameSize & 1)
		{
			DEL_CLASS(pf);
			return nullptr;
		}
		dt.SetValue(ReadUInt16(&recBuff[6]), recBuff[8], recBuff[9], recBuff[10], recBuff[11], recBuff[12], 0, 36);
		Text::StrUTF16_UTF8C(fileName, (const UTF16Char*)&recBuff[15], (UIntOS)fnameSize >> 1);
		fileName[fnameSize >> 1] = 0;
		pf->AddData(fd, currOfst + 15 + fnameSize, recSize - fnameSize - 15, IO::PackFileItem::HeaderType::No, {fileName, (UIntOS)fnameSize >> 1}, Data::Timestamp(dt.ToInstant(), 0), 0, 0, 0);

		currOfst += recSize;
	}

	return pf;
}
