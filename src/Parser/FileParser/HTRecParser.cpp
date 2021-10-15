#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
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

void Parser::FileParser::HTRecParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ReadingDB)
	{
		selector->AddFilter((const UTF8Char*)"*.HTRec", (const UTF8Char*)"HTRec File");
	}
}

IO::ParserType Parser::FileParser::HTRecParser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

IO::ParsedObject *Parser::FileParser::HTRecParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 buff[96];
	if (fd->GetRealData(0, 96, buff) != 96)
	{
		return 0;
	}
	if (*(Int32*)&buff[0] != *(Int32*)"$#\" ")
	{
		return 0;
	}
	UInt32 recCnt = ReadUInt16(&buff[83]);
	if (fd->GetDataSize() != 96 + recCnt * 3)
		return 0;

	Media::HTRecFile *file;
	NEW_CLASS(file, Media::HTRecFile(fd));
	return file;
}
