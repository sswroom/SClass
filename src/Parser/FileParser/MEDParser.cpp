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

void Parser::FileParser::MEDParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.med"), CSTR("MED Package File"));
	}
}

IO::ParserType Parser::FileParser::MEDParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::MEDParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt8 *recBuff;
	UInt32 recCnt;
	UInt32 recSize;
	UInt32 i;
	UInt32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	UTF8Char *sptr;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".MED")))
	{
		return 0;
	}
	if (hdr[0] != 'M' || hdr[1] != 'D')
		return 0;
	recCnt = ReadUInt16(&hdr[6]);
	recSize = ReadUInt16(&hdr[4]);
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
	Text::Encoding enc(932);
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
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], recSize - 8, 0);
		pf->AddData(fd, fileOfst, fileSize, CSTRP(fileName, sptr), 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += recSize;
	}

	MemFree(recBuff);
	return pf;
}
