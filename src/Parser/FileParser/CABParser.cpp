#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/CABParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::CABParser::CABParser()
{
}

Parser::FileParser::CABParser::~CABParser()
{
}

Int32 Parser::FileParser::CABParser::GetName()
{
	return *(Int32*)"CABP";
}

void Parser::FileParser::CABParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.cab"), CSTR("CAB File"));
	}
}

IO::ParserType Parser::FileParser::CABParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::CABParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
//	UInt32 coffFiles;
//	UInt16 cFolders;
//	UInt16 cFiles;

	UInt32 dataOfst;
	UInt32 recSize;
	UInt32 recCnt;
	UInt32 i;
	Int32 j;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF8Char fileName[256];
	UTF8Char *sptr;

	if (ReadInt32(&hdr[0]) != 0x4643534d || ReadInt32(&hdr[4]) != 0 || ReadInt32(&hdr[12]) != 0 || ReadInt32(&hdr[20]) != 0)
		return 0;
	if (ReadUInt32(&hdr[4]) != fd->GetDataSize())
		return 0;
	return 0;
	/////////////////////////////////////
//	coffFiles = ReadUInt32(&hdrBuff[16]);
//	cFolders = ReadUInt16(&hdrBuff[26]);
//	cFiles = ReadUInt16(&hdrBuff[28]);

	dataOfst = ReadUInt32(&hdr[4]);
	recSize = ReadUInt32(&hdr[8]);
	if (recSize % 40 != 0 || dataOfst > fd->GetDataSize())
		return 0;
	if (dataOfst - recSize != 273)
		return 0;
	Text::Encoding enc(932);
	sptr = enc.UTF8FromBytes(fileName, &hdr[12], 255, 0);
	if (!fd->GetFullName()->EndsWith(fileName, (UOSInt)(sptr - fileName)))
	{
		return 0;
	}
	recCnt = recSize / 40;
	if (recCnt == 0)
	{
		return 0;
	}

	Data::ByteBuffer recBuff(recSize);
	if (fd->GetRealData(273, recSize, recBuff) != recSize)
	{
		return 0;
	}

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	
	j = 0;
	i = 0;
	nextOfst = 0;
	while (i < recCnt)
	{
		fileOfst = ReadUInt32(&recBuff[j + 32]);
		fileSize = ReadUInt32(&recBuff[j + 36]);
		if (fileOfst != nextOfst)
		{
			DEL_CLASS(pf);
			return 0;
		}
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], 32, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, CSTRP(fileName, sptr), 0, 0, 0, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += 40;
	}
	return pf;
}
