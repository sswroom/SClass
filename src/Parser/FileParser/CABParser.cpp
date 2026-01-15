#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/VirtualPackageFileFast.h"
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

void Parser::FileParser::CABParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::CABParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
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
	UnsafeArray<UTF8Char> sptr;

	if (ReadInt32(&hdr[0]) != 0x4643534d || ReadInt32(&hdr[4]) != 0 || ReadInt32(&hdr[12]) != 0 || ReadInt32(&hdr[20]) != 0)
		return nullptr;
	if (ReadUInt32(&hdr[4]) != fd->GetDataSize())
		return nullptr;
	return nullptr;
	/////////////////////////////////////
//	coffFiles = ReadUInt32(&hdrBuff[16]);
//	cFolders = ReadUInt16(&hdrBuff[26]);
//	cFiles = ReadUInt16(&hdrBuff[28]);

	dataOfst = ReadUInt32(&hdr[4]);
	recSize = ReadUInt32(&hdr[8]);
	if (recSize % 40 != 0 || dataOfst > fd->GetDataSize())
		return nullptr;
	if (dataOfst - recSize != 273)
		return nullptr;
	Text::Encoding enc(932);
	sptr = enc.UTF8FromBytes(fileName, &hdr[12], 255, 0);
	if (!fd->GetFullName()->EndsWith(fileName, (UIntOS)(sptr - fileName)))
	{
		return nullptr;
	}
	recCnt = recSize / 40;
	if (recCnt == 0)
	{
		return nullptr;
	}

	Data::ByteBuffer recBuff(recSize);
	if (fd->GetRealData(273, recSize, recBuff) != recSize)
	{
		return nullptr;
	}

	IO::VirtualPackageFile *pf;
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
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
			return nullptr;
		}
		sptr = enc.UTF8FromBytes(fileName, &recBuff[j], 32, 0);
		pf->AddData(fd, fileOfst + (UInt64)dataOfst, fileSize, IO::PackFileItem::HeaderType::No, CSTRP(fileName, sptr), nullptr, nullptr, nullptr, 0);

		nextOfst = fileOfst + fileSize;
		i++;
		j += 40;
	}
	return pf;
}
