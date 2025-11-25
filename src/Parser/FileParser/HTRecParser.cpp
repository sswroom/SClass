#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Media/HTRecFile.h"
#include "Parser/FileParser/HTRecParser.h"

Parser::FileParser::HTRecParser::HTRecParser()
{
}

Parser::FileParser::HTRecParser::~HTRecParser()
{
}

Int32 Parser::FileParser::HTRecParser::GetName()
{
	return *(Int32*)"HTRP";
}

void Parser::FileParser::HTRecParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ReadingDB)
	{
		selector->AddFilter(CSTR("*.HTRec"), CSTR("HTRec File"));
	}
}

IO::ParserType Parser::FileParser::HTRecParser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

Optional<IO::ParsedObject> Parser::FileParser::HTRecParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (*(Int32*)&hdr[0] != *(Int32*)"$#\" ")
	{
		return 0;
	}
	UInt32 recCnt = ReadUInt16(&hdr[83]);
	if (fd->GetDataSize() != 96 + recCnt * 3)
		return 0;

	Media::HTRecFile *file;
	NEW_CLASS(file, Media::HTRecFile(fd));
	return file;
}
