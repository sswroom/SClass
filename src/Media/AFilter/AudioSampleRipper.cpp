#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Math/Math_C.h"
#include "Media/AFilter/AudioSampleRipper.h"
#include "Sync/MutexUsage.h"

void Media::AFilter::AudioSampleRipper::ResetStatus()
{
	UnsafeArray<UInt8> soundBuff;
	if (this->soundBuff.SetTo(soundBuff))
	{
		Sync::MutexUsage mutUsage(this->mut);
		MemClear(soundBuff.Ptr(), this->soundBuffLeng);
		this->soundBuffOfst = 0;
		this->changed = true;
	}
}

Media::AFilter::AudioSampleRipper::AudioSampleRipper(NN<Media::AudioSource> sourceAudio, UInt32 sampleCount) : Media::AudioFilter(sourceAudio)
{
	Media::AudioFormat fmt;
	this->soundBuff = 0;
	this->nChannel = 0;
	this->bitCount = 0;
	sourceAudio->GetFormat(fmt);
	if (fmt.formatId != 0x1)
		return;
	this->nChannel = fmt.nChannels;
	this->bitCount = fmt.bitpersample;
	this->soundBuffLeng = sampleCount * fmt.align;
	this->soundBuff = MemAlloc(UInt8, this->soundBuffLeng);
	this->ResetStatus();
}

Media::AFilter::AudioSampleRipper::~AudioSampleRipper()
{
	UnsafeArray<UInt8> soundBuff;
	if (this->soundBuff.SetTo(soundBuff))
	{
		MemFreeArr(soundBuff);
		this->soundBuff = 0;
	}
}

Data::Duration Media::AFilter::AudioSampleRipper::SeekToTime(Data::Duration time)
{
	this->ResetStatus();
	return this->sourceAudio->SeekToTime(time);
}

UOSInt Media::AFilter::AudioSampleRipper::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);
	UnsafeArray<UInt8> soundBuff;
	UOSInt thisSize;
	UOSInt sizeLeft;
	if (this->soundBuff.SetTo(soundBuff))
	{
		Sync::MutexUsage mutUsage(this->mut);
		sizeLeft = readSize;
		while (sizeLeft > 0)
		{
			thisSize = sizeLeft;
			if (thisSize + this->soundBuffOfst > this->soundBuffLeng)
			{
				thisSize = this->soundBuffLeng - this->soundBuffOfst;
			}
			MemCopyNO(&soundBuff[this->soundBuffOfst], blk.Arr().Ptr(), thisSize);
			this->soundBuffOfst = (UInt32)(this->soundBuffOfst + thisSize) % this->soundBuffLeng;
			sizeLeft -= thisSize;
			blk += thisSize;
		}
		this->changed = true;
	}
	return readSize;
}

Bool Media::AFilter::AudioSampleRipper::IsChanged()
{
	if (this->changed)
	{
		this->changed = false;
		return true;
	}
	return false;
}

Bool Media::AFilter::AudioSampleRipper::GetSamples(UnsafeArray<UInt8> samples)
{
	Sync::MutexUsage mutUsage(this->mut);
	UnsafeArray<UInt8> soundBuff;
	if (this->soundBuff.SetTo(soundBuff))
	{
		MemCopyNO(samples.Ptr(), &soundBuff[this->soundBuffOfst], this->soundBuffLeng - this->soundBuffOfst);
		samples += this->soundBuffLeng - this->soundBuffOfst;
		if (this->soundBuffOfst > 0)
		{
			MemCopyNO(samples.Ptr(), soundBuff.Ptr(), this->soundBuffOfst);
		}
		mutUsage.EndUse();
		return true;
	}
	return false;
}
