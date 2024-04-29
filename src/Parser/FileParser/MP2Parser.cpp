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

void Parser::FileParser::MP2Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.mp2"), CSTR("MPEG-1 Audio Layer 2 File"));
	}
}

IO::ParserType Parser::FileParser::MP2Parser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::MP2Parser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (hdr[0] != 0xff || (hdr[1] & 0xfe) != 0xfc)
	{
		return 0;
	}
	
	NN<Media::AudioBlockSource> src;
	Media::MediaFile *vid;
	Media::BlockParser::MP2BlockParser mp2Parser;
	if (src.Set(mp2Parser.ParseStreamData(fd)))
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
