#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/VirtualPackageFileFast.h"
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

void Parser::FileParser::MEDParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::MEDParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt32 recCnt;
	UInt32 recSize;
	UInt32 i;
	UInt32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	UnsafeArray<UTF8Char> sptr;

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

	Data::ByteBuffer recBuff(recSize * recCnt);
	if (fd->GetRealData(16, recSize * recCnt, recBuff) != recSize * recCnt)
	{
		return 0;
	}

	IO::VirtualPackageFile *pf;
	Text::Encoding enc(932);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 16 + recCnt * recSize;
	while (i < recCnt)
	{
		fileSize = ReadUInt32(&recBuff[j + recSize - 8]);
		fileOfst = ReadUInt32(&recBuff[j + recSize - 4]);
		if (fileOfst != nextOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], recSize - 8, 0);
		pf->AddData(fd, fileOfst, fileSize, IO::PackFileItem::HeaderType::No, CSTRP(fileName, sptr), 0, 0, 0, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += recSize;
	}
	return pf;
}
