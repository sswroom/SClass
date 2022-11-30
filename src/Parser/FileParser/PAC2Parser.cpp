#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/PAC2Parser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::PAC2Parser::PAC2Parser()
{
}

Parser::FileParser::PAC2Parser::~PAC2Parser()
{
}

Int32 Parser::FileParser::PAC2Parser::GetName()
{
	return *(Int32*)"PAC2";
}

void Parser::FileParser::PAC2Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.pac"), CSTR("PAC Package File"));
	}
}

IO::ParserType Parser::FileParser::PAC2Parser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::PAC2Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdrBuff[16];
	UInt32 recCnt;
	UInt32 dataOfst;
	UTF8Char fileName[256];
	UTF8Char *sptr;
	Text::Encoding enc(932);

	Int64 ofst;
	UInt8 *recBuff;
	UInt32 i;
	UInt32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 fnameSize;
	UInt32 nextOfst;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".PAC")))
	{
		return 0;
	}
	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (ReadUInt32(&hdrBuff[0]) != 0xad82cf82)
		return 0;
	recCnt = ReadUInt32(&hdrBuff[4]);
	dataOfst = ReadUInt32(&hdrBuff[8]);
	if (recCnt == 0)
	{
		return 0;
	}
	ofst = 16 + recCnt * 268;
	if (ofst & 15)
		ofst = ofst + 16 - (ofst & 15);
	if (dataOfst != ofst)
		return 0;
	recBuff = MemAlloc(UInt8, 268 * recCnt);
	if (fd->GetRealData(16, 268 * recCnt, recBuff) != 268 * recCnt)
	{
		MemFree(recBuff);
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 256]);
		fileSize = ReadUInt32(&recBuff[j + 260]);
		fnameSize = ReadUInt32(&recBuff[j + 264]);
		if (fileOfst != nextOfst)
		{
			MemFree(recBuff);
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], fnameSize, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, CSTRP(fileName, sptr), Data::Timestamp(0));

		nextOfst = fileOfst + fileSize;
		if (nextOfst & 15)
			nextOfst = nextOfst + 16 - (nextOfst & 15);
		i++;
		j += 268;
	}

	MemFree(recBuff);
	return pf;
}
