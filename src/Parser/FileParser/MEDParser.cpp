#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/MEDParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::MEDParser::MEDParser()
{
}

Parser::FileParser::MEDParser::~MEDParser()
{
}

Int32 Parser::FileParser::MEDParser::GetName()
{
	return *(Int32*)"MEDP";
}

void Parser::FileParser::MEDParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.med", (const UTF8Char*)"MED Package File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::MEDParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::MEDParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdrBuff[16];
	UInt8 *recBuff;
	UInt32 recCnt;
	UInt32 recSize;
	UInt32 i;
	UInt32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	Text::Encoding enc(932);

	if (!Text::StrEndsWithICase(fd->GetFullName(), (const UTF8Char*)".MED"))
	{
		return 0;
	}
	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (hdrBuff[0] != 'M' || hdrBuff[1] != 'D')
		return 0;
	recCnt = ReadUInt16(&hdrBuff[6]);
	recSize = ReadUInt16(&hdrBuff[4]);
	if (recSize < 9 || recSize > 32)
		return 0;
	if (recCnt == 0)
	{
		return 0;
	}
	if (recSize * recCnt >= fd->GetDataSize())
		return 0;

	recBuff = MemAlloc(UInt8, recSize * recCnt);
	if (fd->GetRealData(16, recSize * recCnt, recBuff) != recSize * recCnt)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 16 + recCnt * recSize;
	while (i < recCnt)
	{
		fileSize = ReadUInt32(&recBuff[j + recSize - 8]);
		fileOfst = ReadUInt32(&recBuff[j + recSize - 4]);
		if (fileOfst != nextOfst)
		{
			MemFree(recBuff);
			DEL_CLASS(pf);
			return 0;
		}
		enc.UTF8FromBytes(fileName, &recBuff[j], recSize - 8, 0);
		pf->AddData(fd, fileOfst, fileSize, fileName, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += recSize;
	}

	MemFree(recBuff);
	return pf;
}
