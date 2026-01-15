#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Media/MediaFile.h"
#include "Media/AudioFixBlockSource.h"
#include "Parser/FileParser/ADXParser.h"

Parser::FileParser::ADXParser::ADXParser()
{
}

Parser::FileParser::ADXParser::~ADXParser()
{
}

Int32 Parser::FileParser::ADXParser::GetName()
{
	return *(Int32*)"ADXP";
}

IO::ParserType Parser::FileParser::ADXParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

void Parser::FileParser::ADXParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.adx"), CSTR("ADX ADPCM File"));
	}
}

Optional<IO::ParsedObject> Parser::FileParser::ADXParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt32 startOfst;
	UInt8 buff[2048];
	Media::AudioFormat af;
	startOfst = ReadMUInt32(&hdr[0]);
	if ((startOfst & 0x80000000) == 0)
		return nullptr;
	startOfst ^= 0x80000000;
	if (startOfst > 2048)
		return nullptr;
	fd->GetRealData(4, startOfst, BYTEARR(buff));

	
	af.formatId = 0x2080;
	af.nChannels = buff[3];
	af.frequency = ReadMUInt32(&buff[4]);
	af.bitpersample = 16;
	af.bitRate = (af.frequency * af.nChannels * 9) >> 1;
	af.align = (UInt32)18 * af.nChannels;
	af.other = 0;
	af.intType = Media::AudioFormat::IT_NORMAL;
	af.extraSize = 0;
	af.extra = 0;

	Media::MediaFile *vid;
	NN<Media::AudioFixBlockSource> src;
	NEW_CLASSNN(src, Media::AudioFixBlockSource(fd, startOfst + 4, ((ReadMUInt32(&buff[8]) >> 4) + 1) * 18, af, fd->GetFullName()));

	NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
	vid->AddSource(src, 0);
	return vid;
}
