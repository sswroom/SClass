#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/SilentSource.h"
#include "Text/MyString.h"

Media::SilentSource::SilentSource(UInt32 sampleRate, UInt16 nChannels, UInt16 bitCount, Text::CString name, UInt64 sampleCnt)
{
	this->format.frequency = sampleRate;
	this->format.bitpersample = bitCount;
	this->format.align = nChannels * (UInt32)bitCount >> 3;
	this->format.extraSize = 0;
	this->format.extra = 0;
	this->format.formatId = 1;
	this->format.intType = Media::AudioFormat::IT_NORMAL;
	this->format.nChannels = nChannels;
	this->format.other = 0;
	this->format.bitRate = sampleRate * bitCount * nChannels;
	this->sampleCnt = sampleCnt;
	this->name = Text::String::NewOrNull(name);
	this->readEvt = 0;
	this->readOfst = 0;
	this->currSample = 0;
}

Media::SilentSource::~SilentSource()
{
	SDEL_STRING(this->name);
}

UTF8Char *Media::SilentSource::GetSourceName(UTF8Char *buff)
{
	if (this->name == 0)
		return 0;
	return this->name->ConcatTo(buff);
}

Bool Media::SilentSource::CanSeek()
{
	return true;
}

Int32 Media::SilentSource::GetStreamTime()
{
	if (this->sampleCnt == 0)
	{
		return -1;
	}
	return (Int32)(this->sampleCnt * 1000 / this->format.frequency);
}

UInt32 Media::SilentSource::SeekToTime(UInt32 time)
{
	this->currSample = time * (UInt64)this->format.frequency / 1000;
	this->readOfst = this->currSample * this->format.align;
	return (UInt32)(this->currSample * 1000 / this->format.frequency);
}

Bool Media::SilentSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	Int32 streamTime = GetStreamTime();
	if (trimTimeEnd == (UInt32)-1)
	{
		streamTime -= (Int32)trimTimeStart;
	}
	else
	{
		streamTime = (Int32)(trimTimeEnd - trimTimeStart);
	}
	this->sampleCnt = (UInt64)streamTime * this->format.frequency / 1000;
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
	UInt64 endOfst;
	if (this->sampleCnt == 0)
	{
		endOfst = this->readOfst + readSize;
	}
	else
	{
		endOfst = this->sampleCnt * this->format.align;
	}
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

UInt32 Media::SilentSource::GetCurrTime()
{
	UInt32 blk = (this->format.nChannels * (UInt32)this->format.bitpersample >> 3);
	return (UInt32)(this->readOfst * 1000 / this->format.frequency / blk);
}

Bool Media::SilentSource::IsEnd()
{
	if (this->sampleCnt == 0)
	{
		return false;
	}
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
