#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "IO/StreamData.h"
#include "IO/Stream.h"
#include "Media/AudioSource.h"
#include "Media/MediaSource.h"
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

void Parser::FileParser::XPCMParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		//selector->AddFilter(L"*.xml", L"XPCM File");
	}
}

IO::ParserType Parser::FileParser::XPCMParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

Optional<IO::ParsedObject> Parser::FileParser::XPCMParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (*(Int32*)&hdr[0] != *(Int32*)"XPCM")
		return nullptr;
	if (ReadUInt64(&hdr[4]) != fd->GetDataSize() - 28)
		return nullptr;

	Media::MediaFile *vid;
	Media::AudioFormat af;
	af.formatId = ReadUInt16(&hdr[12]);
	af.nChannels = ReadUInt16(&hdr[14]);
	af.frequency = ReadUInt32(&hdr[16]);
	af.bitpersample = ReadUInt16(&hdr[26]);
	af.bitRate = af.frequency * af.nChannels * af.bitpersample;
	af.align = af.frequency * af.nChannels * (UInt32)(af.bitpersample >> 3);
	af.other = 0;
	af.intType = Media::AudioFormat::IT_NORMAL;
	af.extraSize = 0;
	af.extra = 0;

	NN<Media::LPCMSource> src;
	NEW_CLASSNN(src, Media::LPCMSource(fd, 28, ReadUInt32(&hdr[4]), af, fd->GetFullName()));

	NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
	vid->AddSource(src, 0);

	return vid;
}
