#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Math/Math_C.h"
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
	this->readEvt = nullptr;
	this->readOfst = 0;
	this->currSample = 0;
}

Media::SilentSource::~SilentSource()
{
	OPTSTR_DEL(this->name);
}

UnsafeArrayOpt<UTF8Char> Media::SilentSource::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	NN<Text::String> s;
	if (!this->name.SetTo(s))
		return nullptr;
	return s->ConcatTo(buff);
}

Bool Media::SilentSource::CanSeek()
{
	return true;
}

Data::Duration Media::SilentSource::GetStreamTime()
{
	if (this->sampleCnt == 0)
	{
		return Data::Duration::Infinity();
	}
	return Data::Duration::FromRatioU64(this->sampleCnt, this->format.frequency);
}

Data::Duration Media::SilentSource::SeekToTime(Data::Duration time)
{
	this->currSample = time.MultiplyU64(this->format.frequency);
	this->readOfst = this->currSample * this->format.align;
	return Data::Duration::FromRatioU64(this->currSample, this->format.frequency);
}

Bool Media::SilentSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	Data::Duration streamTime = GetStreamTime();
	if (trimTimeEnd == (UInt32)-1)
	{
		streamTime = streamTime - (Int32)trimTimeStart;
	}
	else
	{
		streamTime = (Int32)(trimTimeEnd - trimTimeStart);
	}
	this->sampleCnt = streamTime.MultiplyU64(this->format.frequency);
	syncTime.Set(0);
	return true;
}

void Media::SilentSource::GetFormat(NN<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

Bool Media::SilentSource::Start(Optional<Sync::Event> evt, UOSInt blkSize)
{
	NN<Sync::Event> readEvt;
	this->readEvt = evt;
	if (this->readEvt.SetTo(readEvt))
		readEvt->Set();
	return true;
}

void Media::SilentSource::Stop()
{
	this->readEvt = nullptr;
	this->readOfst = 0;
}

UOSInt Media::SilentSource::ReadBlock(Data::ByteArray blk)
{
	NN<Sync::Event> readEvt;
	UOSInt readSize = blk.GetSize() - (blk.GetSize() % this->format.align);
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
			blk[i] = 0x80;
			i++;
		}
	}
	else if (this->format.bitpersample == 16)
	{
		while (i < readSize)
		{
			WriteInt16(&blk[i], 0);
			i += 2;
		}
	}
	else if (this->format.bitpersample == 24)
	{
		while (i < readSize)
		{
			blk[i] = 0;
			blk[i + 1] = 0;
			blk[i + 2] = 0;
			i += 3;
		}
	}
	else if (this->format.bitpersample == 32)
	{
		while (i < readSize)
		{
			WriteInt32(&blk[i], 0);
			i += 4;
		}
	}
	this->readOfst += readSize;
	this->currSample += readSize / this->format.align;
	if (this->readEvt.SetTo(readEvt))
		readEvt->Set();
	return readSize;
}

UOSInt Media::SilentSource::GetMinBlockSize()
{
	return this->format.align;
}

Data::Duration Media::SilentSource::GetCurrTime()
{
	UInt32 blk = (this->format.nChannels * (UInt32)this->format.bitpersample >> 3);
	return Data::Duration::FromRatioU64(this->readOfst, this->format.frequency * blk);
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
