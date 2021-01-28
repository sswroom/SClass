#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/DTSParser.h"
#include "Media/MediaFile.h"
#include "Media/AudioBlockSource.h"
#include "Media/BlockParser/DTSBlockParser.h"

Parser::FileParser::DTSParser::DTSParser()
{
}

Parser::FileParser::DTSParser::~DTSParser()
{
}

Int32 Parser::FileParser::DTSParser::GetName()
{
	return *(Int32*)"DTSP";
}

void Parser::FileParser::DTSParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.dts", (const UTF8Char*)"DTS File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::DTSParser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::DTSParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[256];
	fd->GetRealData(0, 32, buff);
	if (*(Int32*)&buff[0] != 0x180FE7F || (buff[4] & 0xfc) != 0xfc)
	{
		return 0;
	}
	
	Media::AudioBlockSource *src = 0;
	Media::MediaFile *vid;
	Media::BlockParser::DTSBlockParser dtsParser;
	IO::IStreamData *data = fd->GetPartialData(0, fd->GetDataSize());
	src = dtsParser.ParseStreamData(data);
	if (src)
	{
		NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
		vid->AddSource(src, 0);
		return vid;
	}
	else
	{
		DEL_CLASS(data);
		return 0;
	}
}
