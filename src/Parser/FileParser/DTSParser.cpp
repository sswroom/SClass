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

void Parser::FileParser::DTSParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.dts"), CSTR("DTS File"));
	}
}

IO::ParserType Parser::FileParser::DTSParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::DTSParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (ReadUInt32(&hdr[0]) != 0x180FE7F || (hdr[4] & 0xfc) != 0xfc)
	{
		return 0;
	}
	
	Media::AudioBlockSource *src = 0;
	Media::MediaFile *vid;
	Media::BlockParser::DTSBlockParser dtsParser;
	NotNullPtr<IO::StreamData> data = fd->GetPartialData(0, fd->GetDataSize());
	src = dtsParser.ParseStreamData(data);
	data.Delete();
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
