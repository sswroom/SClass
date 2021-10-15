#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "IO/IStreamData.h"
#include "Media/IMediaSource.h"
#include "Media/IAudioSource.h"
#include "Media/MediaFile.h"
#include "Media/LPCMSource.h"
#include "Media/AudioFixBlockSource.h"
#include "Media/BlockParser/AC3BlockParser.h"
#include "Media/BlockParser/MP3BlockParser.h"
#include "Media/BlockParser/MP2BlockParser.h"
#include "Parser/FileParser/WAVParser.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"

Parser::FileParser::WAVParser::WAVParser()
{
}

Parser::FileParser::WAVParser::~WAVParser()
{
}

Int32 Parser::FileParser::WAVParser::GetName()
{
	return *(Int32*)"WAVP";
}

void Parser::FileParser::WAVParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter((const UTF8Char*)"*.wav", (const UTF8Char*)"RIFF Wave File");
	}
}

IO::ParserType Parser::FileParser::WAVParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::WAVParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt32 buff[4];
	UInt32 fileSize;
	UInt64 currPos;
	fd->GetRealData(0, 12, (UInt8*)buff);
	if (buff[0] != *(UInt32*)"RIFF")
		return 0;
	if (buff[2] != *(UInt32*)"WAVE")
		return 0;
	fileSize = (UInt32)buff[1] + 8;

	Media::MediaFile *vid;
	UInt8 *fmt = 0;
	currPos = 12;
	while (currPos < fileSize)
	{
		fd->GetRealData(currPos, 8, (UInt8*)buff);
		
		if (buff[0] == *(UInt32*)"fmt ")
		{
			if (fmt)
				MemFree(fmt);
			fmt = MemAlloc(UInt8, buff[1]);
			fd->GetRealData(currPos + 8, buff[1], fmt);
		}
		else if (buff[0] == *(UInt32*)"data")
		{
			if (fmt)
			{
				if (*(Int16*)fmt == 1)
				{
					Media::AudioFormat af;
					af.formatId = 1;
					af.nChannels = ReadUInt16(&fmt[2]);
					af.frequency = ReadUInt32(&fmt[4]);
					af.bitpersample = ReadUInt16(&fmt[14]);
					af.bitRate = af.frequency * af.nChannels * af.bitpersample;
					af.align = ReadUInt16(&fmt[12]);
					af.other = 0;
					af.intType = Media::AudioFormat::IT_NORMAL;
					af.extraSize = 0;
					af.extra = 0;

					Media::LPCMSource *src;
					NEW_CLASS(src, Media::LPCMSource(fd, currPos + 8, buff[1], &af, fd->GetFullName()));

					NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
					vid->AddSource(src, 0);
					MemFree(fmt);
					return vid;
				}
				else if (*(Int16*)fmt == 0x2000)
				{
					Media::AudioBlockSource *src;
					Media::BlockParser::AC3BlockParser ac3Parser;
					IO::IStreamData *data = fd->GetPartialData(currPos + 8, buff[1]);
					src = ac3Parser.ParseStreamData(data);
					DEL_CLASS(data);
					if (src)
					{
						NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
						vid->AddSource(src, 0);
						MemFree(fmt);
						return vid;
					}
					else
					{
						MemFree(fmt);
						return 0;
					}
				}
				else if (*(Int16*)fmt == 0x55)
				{
					Media::AudioBlockSource *src;
					Media::BlockParser::MP3BlockParser mp3Parser;
					IO::IStreamData *data = fd->GetPartialData(currPos + 8, buff[1]);
					src = mp3Parser.ParseStreamData(data);
					DEL_CLASS(data);
					if (src)
					{
						NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
						vid->AddSource(src, 0);
						MemFree(fmt);
						return vid;
					}
					else
					{
						MemFree(fmt);
						return 0;
					}
				}
				else if (*(Int16*)fmt == 0x50)
				{
					Media::AudioBlockSource *src;
					Media::BlockParser::MP2BlockParser mp2Parser;
					IO::IStreamData *data = fd->GetPartialData(currPos + 8, buff[1]);
					src = mp2Parser.ParseStreamData(data);
					DEL_CLASS(data);
					if (src)
					{
						NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
						vid->AddSource(src, 0);
						MemFree(fmt);
						return vid;
					}
					else
					{
						MemFree(fmt);
						return 0;
					}
				}
				else if (*(Int16*)&fmt[12] > 1)
				{
					Media::AudioFormat af;
					af.FromWAVEFORMATEX(fmt);
					
					Media::AudioFixBlockSource *src;
					NEW_CLASS(src, Media::AudioFixBlockSource(fd, currPos + 8, buff[1], &af, fd->GetFullName()));

					NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
					vid->AddSource(src, 0);
					MemFree(fmt);
					return vid;
				}
			}
		}
		else if (buff[0] & 0x80808080)
		{
			if (fmt)
				MemFree(fmt);
			return 0;
		}

		currPos += (UInt32)buff[1] + 8;
	}
	if (fmt)
		MemFree(fmt);
	return 0;
}
