#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/MP2Parser.h"
#include "Media/MediaFile.h"
#include "Media/AudioBlockSource.h"
#include "Media/BlockParser/MP2BlockParser.h"

Parser::FileParser::MP2Parser::MP2Parser()
{
}

Parser::FileParser::MP2Parser::~MP2Parser()
{
}

Int32 Parser::FileParser::MP2Parser::GetName()
{
	return *(Int32*)"MP2P";
}

void Parser::FileParser::MP2Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.mp2", (const UTF8Char*)"MPEG-1 Audio Layer 2 File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::MP2Parser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::MP2Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[256];
	fd->GetRealData(0, 32, buff);
	if (buff[0] != 0xff || (buff[1] & 0xfe) != 0xfc)
	{
		return 0;
	}
	
	Media::AudioBlockSource *src = 0;
	Media::MediaFile *vid;
	Media::BlockParser::MP2BlockParser mp2Parser;
	src = mp2Parser.ParseStreamData(fd);
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
