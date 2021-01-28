#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/AC3Parser.h"
#include "Media/MediaFile.h"
#include "Media/AudioBlockSource.h"
#include "Media/BlockParser/AC3BlockParser.h"

Parser::FileParser::AC3Parser::AC3Parser()
{
}

Parser::FileParser::AC3Parser::~AC3Parser()
{
}

Int32 Parser::FileParser::AC3Parser::GetName()
{
	return *(Int32*)"AC3P";
}

void Parser::FileParser::AC3Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.ac3", (const UTF8Char*)"Dolby AC-3 File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::AC3Parser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::AC3Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[256];
	fd->GetRealData(0, 32, buff);
	if (buff[0] != 0x0b || buff[1] != 0x77)
	{
		return 0;
	}
	
	Media::AudioBlockSource *src = 0;
	Media::MediaFile *vid;
	Media::BlockParser::AC3BlockParser ac3Parser;
	src = ac3Parser.ParseStreamData(fd);
	if (src)
	{
		NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
		vid->AddSource(src, 0);
		return vid;
	}
	else
	{
		return 0;
	}
}
