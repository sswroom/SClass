#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/NOAParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::NOAParser::NOAParser()
{
}

Parser::FileParser::NOAParser::~NOAParser()
{
}

Int32 Parser::FileParser::NOAParser::GetName()
{
	return *(Int32*)"NOAP";
}

void Parser::FileParser::NOAParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter((const UTF8Char*)"*.noa", (const UTF8Char*)"NOA Package File");
	}
}

IO::ParserType Parser::FileParser::NOAParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::NOAParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdrBuff[64];
	UInt8 tagBuff[16];
	UInt32 tagSize;
	UInt32 dataOfst;
	UInt8 *recBuff;

	UInt32 recCnt;
	UInt32 fnameSize;
	UInt32 i;
	UInt32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	Data::DateTime dt;
	UTF8Char fileName[256];
	Text::Encoding enc(932);

	if (!fd->GetFullName()->EndsWithICase((const UTF8Char*)".NOA"))
	{
		return 0;
	}
	if (fd->GetRealData(0, 64, hdrBuff) != 64)
		return 0;
	if (ReadInt32(&hdrBuff[0]) != 0x69746e45 || ReadInt32(&hdrBuff[4]) != 0x1a73)
		return 0;

	dataOfst = 64;
	fd->GetRealData(64, 16, tagBuff);
	if (ReadInt32(&tagBuff[0]) != 0x45726944 || ReadInt32(&tagBuff[4]) != 0x7972746e)
	{
		return 0;
	}
	tagSize = ReadUInt32(&tagBuff[8]);
	fd->GetRealData(64 + tagSize + 16, 16, tagBuff);
	if (ReadInt32(&tagBuff[0]) != 0x656c6966 || ReadInt32(&tagBuff[4]) != 0x61746164)
	{
		return 0;
	}
	recBuff = MemAlloc(UInt8, tagSize);
	if (fd->GetRealData(64 + 16, tagSize, recBuff) != tagSize)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 4;
	i = 0;
	recCnt = ReadUInt32(&recBuff[0]);
	while (i < recCnt)
	{
		fileSize = ReadUInt32(&recBuff[j]);
		fileOfst = ReadUInt32(&recBuff[j + 16]);
		fnameSize = ReadUInt32(&recBuff[j + 36]);
		dt.SetValue(ReadUInt16(&recBuff[j + 30]), recBuff[j + 29], recBuff[j + 28], recBuff[j + 26], recBuff[j + 25], recBuff[j + 24], 0, 36);
		enc.UTF8FromBytes(fileName, &recBuff[j + 40], fnameSize, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, fileName, dt.ToTicks());

		i++;
		j += 40 + fnameSize;
	}

	MemFree(recBuff);
	return pf;
}
