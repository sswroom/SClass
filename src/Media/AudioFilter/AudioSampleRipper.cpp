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
	mutUsage.EndUse();
}

Media::AudioFilter::AudioSampleRipper::AudioSampleRipper(Media::IAudioSource *sourceAudio, UInt32 sampleCount) : Media::IAudioFilter(sourceAudio)
{
	Media::AudioFormat fmt;
	this->soundBuff = 0;
	NEW_CLASS(this->mut, Sync::Mutex());
	sourceAudio->GetFormat(&fmt);
	if (fmt.formatId != 0x1)
		return;
	this->sourceAudio = sourceAudio;
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
	DEL_CLASS(this->mut);
}

Int32 Media::AudioFilter::AudioSampleRipper::SeekToTime(Int32 time)
{
	if (this->sourceAudio)
	{
		this->ResetStatus();
		return this->sourceAudio->SeekToTime(time);
	}
	return 0;
}

UOSInt Media::AudioFilter::AudioSampleRipper::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	if (this->sourceAudio == 0)
		return 0;

	UOSInt readSize = this->sourceAudio->ReadBlock(buff, blkSize);
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
		MemCopyNO(&this->soundBuff[this->soundBuffOfst], buff, thisSize);
		this->soundBuffOfst = (UInt32)(this->soundBuffOfst + thisSize) % this->soundBuffLeng;
		sizeLeft -= thisSize;
		buff += thisSize;
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
