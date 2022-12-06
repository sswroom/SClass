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

void Parser::FileParser::AC3Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.ac3"), CSTR("Dolby AC-3 File"));
	}
}

IO::ParserType Parser::FileParser::AC3Parser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::AC3Parser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (hdr[0] != 0x0b || hdr[1] != 0x77)
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
