#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/BSAParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::BSAParser::BSAParser()
{
}

Parser::FileParser::BSAParser::~BSAParser()
{
}

Int32 Parser::FileParser::BSAParser::GetName()
{
	return *(Int32*)"BSAP";
}

void Parser::FileParser::BSAParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.bsa", (const UTF8Char*)"BSA Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::BSAParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::BSAParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdrBuff[16];
//	UInt16 ver;
	UInt16 recCnt;
	UInt32 recOfst;

	UOSInt recSize;
	UInt8 *recBuff;
	UInt32 i;
	Int32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
//	UInt32 nextOfst;
	UTF8Char fileName[512];
	Text::Encoding enc(932);

	if (!Text::StrEndsWithICase(fd->GetFullName(), (const UTF8Char*)".BSA"))
	{
		return 0;
	}
	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (ReadInt32(&hdrBuff[0]) != 0x72415342 || ReadInt32(&hdrBuff[4]) != 0x63)
		return 0;
//	ver = ReadUInt16(&hdrBuff[8]);
	recCnt = ReadUInt16(&hdrBuff[10]);
	recOfst = ReadUInt32(&hdrBuff[12]);

	if (recOfst >= fd->GetDataSize() - recCnt * 12)
		return 0;

	recSize = (UOSInt)(fd->GetDataSize() - recOfst);
	recBuff = MemAlloc(UInt8, recSize);
	if (fd->GetRealData(recOfst, recSize, recBuff) != recSize)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 4]);
		fileSize = ReadUInt32(&recBuff[j + 8]);
		if (fileSize == 0 && fileOfst == 0)
		{
		}
		else
		{
			enc.UTF8FromBytes(fileName, &recBuff[recCnt * 12 + ReadInt32(&recBuff[j])], -1, 0);
			pf->AddData(fd, fileOfst, fileSize, fileName, 0);
		}
		i++;
		j += 12;
	}

	MemFree(recBuff);
	return pf;
}
