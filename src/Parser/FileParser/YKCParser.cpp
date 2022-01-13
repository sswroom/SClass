#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/YKCParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::YKCParser::YKCParser()
{
}

Parser::FileParser::YKCParser::~YKCParser()
{
}

Int32 Parser::FileParser::YKCParser::GetName()
{
	return *(Int32*)"YKCP";
}

void Parser::FileParser::YKCParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter((const UTF8Char*)"*.ykc", (const UTF8Char*)"YKC Package File");
	}
}

IO::ParserType Parser::FileParser::YKCParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::YKCParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdrBuff[24];
	UInt32 recOfst;
	UInt32 recSize;
	UInt8 *recBuff;

	UInt32 i;
	UInt32 fnameOfst;
    UInt32 fnameSize;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UInt8 fnameBuff[256];
	UTF8Char fileName[256];
	Text::Encoding enc(932);

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".YKC")))
	{
		return 0;
	}
	if (fd->GetRealData(0, 24, hdrBuff) != 24)
		return 0;
	if (ReadInt32(&hdrBuff[0]) != 0x30434b59 || ReadInt32(&hdrBuff[4]) != 0x3130)
		return 0;
	if (ReadUInt32(&hdrBuff[8]) != 24)
		return 0;
	recOfst = ReadUInt32(&hdrBuff[16]);
	recSize = ReadUInt32(&hdrBuff[20]);
	if (recOfst + recSize != fd->GetDataSize())
		return 0;
	recBuff = MemAlloc(UInt8, recSize);
	if (fd->GetRealData(recOfst, recSize, recBuff) != recSize)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	i = 0;
	nextOfst = 24;
	while (i < recSize)
	{
		fnameOfst = ReadUInt32(&recBuff[i]);
		fnameSize = ReadUInt32(&recBuff[i + 4]);
		fileOfst = ReadUInt32(&recBuff[i + 8]);
		fileSize = ReadUInt32(&recBuff[i + 12]);
		if (fileOfst != nextOfst || fnameSize == 0 || fnameSize >= 256)
		{
			MemFree(recBuff);
			DEL_CLASS(pf);
			return 0;
		}
		fd->GetRealData(fnameOfst, fnameSize, fnameBuff);
		enc.UTF8FromBytes(fileName, fnameBuff, fnameSize, 0);
		pf->AddData(fd, fileOfst, fileSize, fileName, 0);

		nextOfst = fileOfst + fileSize;
		i += 20;
	}

	MemFree(recBuff);
	return pf;
}
