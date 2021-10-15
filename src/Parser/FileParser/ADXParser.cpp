#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
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

void Parser::FileParser::ADXParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter((const UTF8Char*)"*.adx", (const UTF8Char*)"ADX ADPCM File");
	}
}

IO::ParsedObject *Parser::FileParser::ADXParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt32 startOfst;
	UInt8 buff[2048];
	Media::AudioFormat af;
	fd->GetRealData(0, 4, buff);
	startOfst = ReadMUInt32(buff);
	if ((startOfst & 0x80000000) == 0)
		return 0;
	startOfst ^= 0x80000000;
	if (startOfst > 2048)
		return 0;
	fd->GetRealData(4, startOfst, buff);

	
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
	Media::AudioFixBlockSource *src;
	NEW_CLASS(src, Media::AudioFixBlockSource(fd, startOfst + 4, ((ReadMUInt32(&buff[8]) >> 4) + 1) * 18, &af, fd->GetFullName()));

	NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
	vid->AddSource(src, 0);
	return vid;
}
