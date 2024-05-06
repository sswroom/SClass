#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/AudioFilter/AudioSampleRipper.h"
#include "Sync/MutexUsage.h"

void Media::AudioFilter::AudioSampleRipper::ResetStatus()
{
	Sync::MutexUsage mutUsage(this->mut);
	MemClear(this->soundBuff, this->soundBuffLeng);
	this->soundBuffOfst = 0;
	this->changed = true;
}

Media::AudioFilter::AudioSampleRipper::AudioSampleRipper(NN<Media::IAudioSource> sourceAudio, UInt32 sampleCount) : Media::IAudioFilter(sourceAudio)
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

Media::AudioFilter::AudioSampleRipper::~AudioSampleRipper()
{
	if (this->soundBuff)
	{
		MemFree(this->soundBuff);
		this->soundBuff = 0;
	}
}

Data::Duration Media::AudioFilter::AudioSampleRipper::SeekToTime(Data::Duration time)
{
	this->ResetStatus();
	return this->sourceAudio->SeekToTime(time);
}

UOSInt Media::AudioFilter::AudioSampleRipper::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);
	UOSInt thisSize;
	UOSInt sizeLeft;
	Sync::MutexUsage mutUsage(this->mut);
	sizeLeft = readSize;
	while (sizeLeft > 0)
	{
		thisSize = sizeLeft;
		if (thisSize + this->soundBuffOfst > this->soundBuffLeng)
		{
			thisSize = this->soundBuffLeng - this->soundBuffOfst;
		}
		MemCopyNO(&this->soundBuff[this->soundBuffOfst], blk.Ptr().Ptr(), thisSize);
		this->soundBuffOfst = (UInt32)(this->soundBuffOfst + thisSize) % this->soundBuffLeng;
		sizeLeft -= thisSize;
		blk += thisSize;
	}
	this->changed = true;
	mutUsage.EndUse();
	return readSize;
}

Bool Media::AudioFilter::AudioSampleRipper::IsChanged()
{
	if (this->changed)
	{
		this->changed = false;
		return true;
	}
	return false;
}

Bool Media::AudioFilter::AudioSampleRipper::GetSamples(UInt8 *samples)
{
	Sync::MutexUsage mutUsage(this->mut);
	MemCopyNO(samples, &this->soundBuff[this->soundBuffOfst], this->soundBuffLeng - this->soundBuffOfst);
	samples += this->soundBuffLeng - this->soundBuffOfst;
	if (this->soundBuffOfst > 0)
	{
		MemCopyNO(samples, this->soundBuff, this->soundBuffOfst);
	}
	mutUsage.EndUse();
	return true;
}
