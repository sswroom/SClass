#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/IStreamData.h"
#include "IO/Stream.h"
#include "Media/IAudioSource.h"
#include "Media/IMediaSource.h"
#include "Media/LPCMSource.h"
#include "Media/MediaFile.h"
#include "Parser/FileParser/XPCMParser.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"

Parser::FileParser::XPCMParser::XPCMParser()
{
}

Parser::FileParser::XPCMParser::~XPCMParser()
{
}

Int32 Parser::FileParser::XPCMParser::GetName()
{
	return *(Int32*)"XPCM";
}

void Parser::FileParser::XPCMParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
		//selector->AddFilter(L"*.xml", L"XPCM File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::XPCMParser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::XPCMParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[28];
	fd->GetRealData(0, 28, buff);
	if (*(Int32*)&buff[0] != *(Int32*)"XPCM")
		return 0;
	if (ReadUInt64(&buff[4]) != fd->GetDataSize() - 28)
		return 0;

	Media::MediaFile *vid;
	Media::AudioFormat af;
	af.formatId = *(Int16*)&buff[12];
	af.nChannels = *(Int16*)&buff[14];
	af.frequency = *(Int32*)&buff[16];
	af.bitpersample = *(Int16*)&buff[26];
	af.bitRate = af.frequency * af.nChannels * af.bitpersample;
	af.align = af.frequency * af.nChannels * (af.bitpersample >> 3);
	af.other = 0;
	af.intType = Media::AudioFormat::IT_NORMAL;
	af.extraSize = 0;
	af.extra = 0;

	Media::LPCMSource *src;
	NEW_CLASS(src, Media::LPCMSource(fd, 28, *(Int32*)&buff[4], &af, fd->GetFullName()));

	NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
	vid->AddSource(src, 0);

	return vid;
}
