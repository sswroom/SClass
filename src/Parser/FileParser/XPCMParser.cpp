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

void Parser::FileParser::XPCMParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::XPCMParser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (*(Int32*)&hdr[0] != *(Int32*)"XPCM")
		return 0;
	if (ReadUInt64(&hdr[4]) != fd->GetDataSize() - 28)
		return 0;

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

	Media::LPCMSource *src;
	NEW_CLASS(src, Media::LPCMSource(fd, 28, ReadUInt32(&hdr[4]), &af, fd->GetFullName()));

	NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
	vid->AddSource(src, 0);

	return vid;
}
