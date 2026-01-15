#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/SakuotoArcParser.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

Parser::FileParser::SakuotoArcParser::SakuotoArcParser()
{
}

Parser::FileParser::SakuotoArcParser::~SakuotoArcParser()
{
}

Int32 Parser::FileParser::SakuotoArcParser::GetName()
{
	return *(Int32*)"SARC";
}

void Parser::FileParser::SakuotoArcParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.arc"), CSTR("ARC Package File"));
	}
}

IO::ParserType Parser::FileParser::SakuotoArcParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::SakuotoArcParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt64 dataOfst;
	UInt32 recCnt;
	UInt32 recSize;
	UInt32 i;
	UInt32 fileSize;
	UInt32 fileOfst;
	UInt32 nextOfst;
	UTF16Char *fileName;

	if (ReadInt32(&hdr[12]) != 0)
		return nullptr;
	recCnt = ReadUInt32(&hdr[0]);
	recSize = ReadUInt32(&hdr[4]);
	if (recCnt == 0 || recCnt >= 65536)
		return nullptr;
	if (recSize < recCnt * 10 || recSize >= 1048576)
	{
		return nullptr;
	}

	Data::ByteBuffer recBuff(recSize);
	dataOfst = recSize + 8;
	if (fd->GetRealData(8, recSize, recBuff) != recSize)
	{
		return nullptr;
	}

	IO::VirtualPackageFile *pf;
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	
	i = 0;
	nextOfst = 0;
	while (i < recSize)
	{
		fileSize = ReadUInt32(&recBuff[i]);
		fileOfst = ReadUInt32(&recBuff[i + 4]);
		if (fileOfst != nextOfst)
		{
			DEL_CLASS(pf);
			return nullptr;
		}
		fileName = (UTF16Char*)&recBuff[i + 8];
		while (*fileName++);
		NN<Text::String> s = Text::String::NewNotNull((UTF16Char*)&recBuff[i + 8]);
		pf->AddData(fd, dataOfst + fileOfst, fileSize, IO::PackFileItem::HeaderType::No, s->ToCString(), 0, 0, 0, 0);
		s->Release();

		nextOfst = fileOfst + fileSize;
		i = (UInt32)(((UInt8*)fileName) - recBuff.Arr());
	}
	return pf;
}
