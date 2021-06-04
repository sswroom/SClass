#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "IO/StmData/ConcatStreamData.h"
#include "IO/StmData/FileData.h"
#include "Parser/FileParser/NS2Parser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::NS2Parser::NS2Parser()
{
}

Parser::FileParser::NS2Parser::~NS2Parser()
{
}

Int32 Parser::FileParser::NS2Parser::GetName()
{
	return *(Int32*)"NS2P";
}

void Parser::FileParser::NS2Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.ns2", (const UTF8Char*)"NS2 Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::NS2Parser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::NS2Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 testBuff[256];
	UInt8 *hdrBuff;
	UInt32 hdrSize;
	Text::Encoding enc(932);
	UInt64 fileOfst;
	UInt32 fileSize;
	UTF8Char sbuff[512];
	UOSInt i;
	UOSInt j;

	fd->GetRealData(0, 256, testBuff);
	if (testBuff[4] != '"')
		return 0;
	i = 5;
	while (i < 32)
	{
		if (testBuff[i] == '"')
			break;
		i++;
	}
	if (i >= 32)
		return 0;
	if (testBuff[i + 5] != '"')
		return 0;

	IO::PackageFile *pf;
	hdrSize = ReadUInt32(&testBuff[0]);
	hdrBuff = MemAlloc(UInt8, hdrSize);
	fd->GetRealData(0, hdrSize, hdrBuff);
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	i = 4;
	fileOfst = hdrSize;
	while ((UInt32)i < hdrSize)
	{
		if (hdrBuff[i] != '"')
			break;

		i++;
		j = i;
		while ((UInt32)i < hdrSize)
		{
			if (hdrBuff[i] == '"')
				break;
			i++;
		}
		if ((UInt32)i >= hdrSize - 4)
			break;
		enc.UTF8FromBytes(sbuff, &hdrBuff[j], i - j, 0);
		fileSize = ReadUInt32(&hdrBuff[i + 1]);
		pf->AddData(fd, fileOfst, fileSize, sbuff, 0);
		fileOfst += fileSize;
		i += 5;
	}

	MemFree(hdrBuff);
	return pf;
}
