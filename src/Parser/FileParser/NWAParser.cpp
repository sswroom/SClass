#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "IO/IStreamData.h"
#include "Media/MediaFile.h"
#include "Media/LPCMSource.h"
#include "Media/NWASource.h"
#include "Parser/FileParser/NWAParser.h"

Parser::FileParser::NWAParser::NWAParser()
{
}

Parser::FileParser::NWAParser::~NWAParser()
{
}

Int32 Parser::FileParser::NWAParser::GetName()
{
	return *(Int32*)"NWAP";
}

void Parser::FileParser::NWAParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.nwa", (const UTF8Char*)"NWA Audio File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::NWAParser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::NWAParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdrBuff[44];
	Int32 compLevel;
	UInt32 nBlocks;
	UInt32 dataSize;
	UInt32 compDataSize;
	UInt32 sampleCount;
	UInt32 blockSize;
	UInt32 restSize;
	Media::AudioFormat afmt;
	if (!Text::StrEndsWithICase(fd->GetFullName(), (const UTF8Char*)".NWA"))
		return 0;
	fd->GetRealData(0, 44, hdrBuff);
	afmt.formatId = 1;
	afmt.nChannels = ReadUInt16(&hdrBuff[0]);
	afmt.bitpersample = ReadUInt16(&hdrBuff[2]);
	afmt.frequency = ReadUInt32(&hdrBuff[4]);
	compLevel = ReadInt32(&hdrBuff[8]);
	nBlocks = ReadUInt32(&hdrBuff[16]);
	dataSize = ReadUInt32(&hdrBuff[20]);
	compDataSize = ReadUInt32(&hdrBuff[24]);
	sampleCount = ReadUInt32(&hdrBuff[28]);
	blockSize = ReadUInt32(&hdrBuff[32]);
	restSize = ReadUInt32(&hdrBuff[36]);
	if (compLevel == -1)
	{
		blockSize = 65536;
		restSize = dataSize % (blockSize * afmt.bitpersample / 8);
		nBlocks = dataSize / (blockSize * afmt.bitpersample / 8) + (restSize > 0 ? 1 : 0);
	}
	afmt.extraSize = 0;
	afmt.align = (UInt32)afmt.nChannels * afmt.bitpersample / 8;
	afmt.bitRate = afmt.frequency * afmt.align * 8;
	afmt.intType = Media::AudioFormat::IT_NORMAL;
	afmt.other = 0;
	if (afmt.nChannels != 1 && afmt.nChannels != 2)
		return 0;
	if (afmt.bitpersample != 8 && afmt.bitpersample != 16)
		return 0;
	if (fd->GetDataSize() != compDataSize)
		return 0;

	UInt32 byps = afmt.bitpersample / 8;
	if (dataSize != sampleCount * byps)
		return 0;
	if (sampleCount != (nBlocks - 1) * blockSize + restSize )
		return 0;
	if (compLevel < -1 || compLevel > 2)
	{
		return 0;
	}

	if (compLevel == -1)
	{
		Media::LPCMSource *asrc;
		Media::MediaFile *file;
		NEW_CLASS(asrc, Media::LPCMSource(fd, 0x2c, compDataSize - 0x2c, &afmt, fd->GetFullName()));
		NEW_CLASS(file, Media::MediaFile(fd->GetFullName()));
		file->AddSource(asrc, 0);
		return file;
	}
	else
	{
		Media::NWASource *asrc;
		Media::MediaFile *file;
		NEW_CLASS(asrc, Media::NWASource(fd, sampleCount, blockSize, (UInt32)compLevel, nBlocks, &afmt, fd->GetFullName()));
		NEW_CLASS(file, Media::MediaFile(fd->GetFullName()));
		file->AddSource(asrc, 0);
		return file;
	}
}
