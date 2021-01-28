#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/SilentSource.h"
#include "Text/MyString.h"

Media::SilentSource::SilentSource(UInt32 sampleRate, UInt32 nChannels, UInt32 bitCount, const UTF8Char *name, UInt64 sampleCnt)
{
	this->format.frequency = sampleRate;
	this->format.bitpersample = bitCount;
	this->format.align = nChannels * bitCount >> 3;
	this->format.extraSize = 0;
	this->format.extra = 0;
	this->format.formatId = 1;
	this->format.intType = Media::AudioFormat::IT_NORMAL;
	this->format.nChannels = nChannels;
	this->format.other = 0;
	this->format.bitRate = sampleRate * bitCount * nChannels;
	this->sampleCnt = sampleCnt;

	if (name)
	{
		this->name = Text::StrCopyNew(name);
	}
	else
	{
		this->name = 0;
	}

	this->readEvt = 0;
	this->readOfst = 0;
	this->currSample = 0;
}

Media::SilentSource::~SilentSource()
{
	if (this->name)
	{
		Text::StrDelNew(this->name);
	}
}

UTF8Char *Media::SilentSource::GetSourceName(UTF8Char *buff)
{
	if (this->name == 0)
		return 0;
	return Text::StrConcat(buff, this->name);
}

Bool Media::SilentSource::CanSeek()
{
	return true;
}

Int32 Media::SilentSource::GetStreamTime()
{
	return (Int32)(this->sampleCnt * 1000 / this->format.frequency);
}

Int32 Media::SilentSource::SeekToTime(Int32 time)
{
	this->currSample = time * (Int64)this->format.frequency / 1000;
	this->readOfst = this->currSample * this->format.align;
	return (Int32)(this->currSample * 1000 / this->format.frequency);
}

Bool Media::SilentSource::TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime)
{
	Int32 streamTime = GetStreamTime();
	if (trimTimeEnd == -1)
	{
		streamTime -= trimTimeStart;
	}
	else
	{
		streamTime = trimTimeEnd - trimTimeStart;
	}
	this->sampleCnt = streamTime * this->format.frequency / 1000;
	if (syncTime)
	{
		*syncTime = 0;
	}
	return true;
}

void Media::SilentSource::GetFormat(AudioFormat *format)
{
	format->FromAudioFormat(&this->format);
}

Bool Media::SilentSource::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->readEvt = evt;
	if (this->readEvt)
		this->readEvt->Set();
	return true;
}

void Media::SilentSource::Stop()
{
	this->readEvt = 0;
	this->readOfst = 0;
}

UOSInt Media::SilentSource::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	UOSInt readSize = blkSize - (blkSize % this->format.align);
	UOSInt i = 0;
	UInt64 endOfst = this->sampleCnt * this->format.align;
	if (this->readOfst >= endOfst)
	{
		readSize = 0;
	}
	else if (this->readOfst + readSize > endOfst)
	{
		readSize = (UOSInt)(endOfst - this->readOfst);
	}
	if (this->format.bitpersample == 8)
	{
		while (i < readSize)
		{
			buff[i] = 0x80;
			i++;
		}
	}
	else if (this->format.bitpersample == 16)
	{
		while (i < readSize)
		{
			WriteInt16(&buff[i], 0);
			i += 2;
		}
	}
	else if (this->format.bitpersample == 24)
	{
		while (i < readSize)
		{
			buff[i] = 0;
			buff[i + 1] = 0;
			buff[i + 2] = 0;
			i += 3;
		}
	}
	else if (this->format.bitpersample == 32)
	{
		while (i < readSize)
		{
			WriteInt32(&buff[i], 0);
			i += 4;
		}
	}
	this->readOfst += readSize;
	this->currSample += readSize / this->format.align;
	if (this->readEvt)
		this->readEvt->Set();
	return readSize;
}

UOSInt Media::SilentSource::GetMinBlockSize()
{
	return this->format.align;
}

Int32 Media::SilentSource::GetCurrTime()
{
	Int32 blk = (this->format.nChannels * this->format.bitpersample >> 3);
	return (Int32)(this->readOfst * 1000 / this->format.frequency / blk);
}

Bool Media::SilentSource::IsEnd()
{
	UInt64 endOfst = this->sampleCnt * this->format.align;
	if (this->readOfst >= endOfst)
	{
		return true;
	}
	else
	{
		return false;
	}
}
