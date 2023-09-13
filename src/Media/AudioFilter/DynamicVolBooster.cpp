#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/AudioFilter/DynamicVolBooster.h"
#include "Sync/MutexUsage.h"

void Media::AudioFilter::DynamicVolBooster::ResetStatus()
{
	UOSInt i = soundBuffLeng;
	while (i-- > 0)
	{
		soundBuff[i] = 0;
	}
	maxVol = 0;
	lastVol = 0;
	thisVol = 0;
	maxIndex = 0;
	soundIndex = 0;
}

Media::AudioFilter::DynamicVolBooster::DynamicVolBooster(NotNullPtr<Media::IAudioSource> sourceAudio) : Media::IAudioFilter(sourceAudio)
{
	Media::AudioFormat fmt;
	this->soundBuff = 0;
	this->bgLevel = 0.001;
	this->bitCount = 0;
	this->nChannels = 0;
	sourceAudio->GetFormat(fmt);
	if (fmt.formatId != 0x1)
		return;
	if (fmt.bitpersample != 16 && fmt.bitpersample != 8)
		return;
	this->soundBuffLeng = fmt.frequency >> 3;
	this->soundBuff = MemAlloc(Int32, this->soundBuffLeng);
	this->nChannels = fmt.nChannels;
	this->bitCount = fmt.bitpersample;
	this->ResetStatus();
}

Media::AudioFilter::DynamicVolBooster::~DynamicVolBooster()
{
	if (this->soundBuff)
	{
		MemFree(this->soundBuff);
		this->soundBuff = 0;
	}
}

void Media::AudioFilter::DynamicVolBooster::GetFormat(NotNullPtr<AudioFormat> format)
{
	Media::AudioFormat fmt;
	this->sourceAudio->GetFormat(fmt);
	format->formatId = 1;
	format->bitpersample = (UInt16)this->bitCount;
	format->frequency = fmt.frequency;
	format->nChannels = fmt.nChannels;
	format->bitRate = fmt.frequency * fmt.nChannels * this->bitCount;
	format->align = fmt.nChannels * this->bitCount >> 3;
	format->other = 0;
	format->intType = Media::AudioFormat::IT_NORMAL;
	format->extraSize = 0;
	format->extra = 0;
}

UInt32 Media::AudioFilter::DynamicVolBooster::SeekToTime(UInt32 time)
{
	this->ResetStatus();
	return this->sourceAudio->SeekToTime(time);
}

UOSInt Media::AudioFilter::DynamicVolBooster::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);
	if (this->enabled)
	{
		if (this->bitCount == 16)
		{
			Sync::MutexUsage mutUsage(this->mut);
			Double log32 = Math_Log10(this->bgLevel * 32768);
			Double log32768 = Math_Log10(32768);
			Int32 noiseVol = Double2Int32(this->bgLevel * 32768);
			UOSInt i = 0;
			UOSInt j;
			Int32 currSample;
			Int32 absVol;
			Int32 thisVol;
			while (i < readSize)
			{
				thisVol = 0;
				j = this->nChannels;
				while (j-- > 0)
				{
					currSample = *(Int16*)&blk[i + j * 2];
					if (currSample < 0)
					{
						absVol = -(Int32)currSample;
					}
					else
					{
						absVol = currSample;
					}
					if (absVol > thisVol)
						thisVol = absVol;
				}
				if (thisVol >= this->maxVol)
				{
					maxVol = thisVol;
					maxIndex = soundIndex;
				}
				soundBuff[soundIndex] = thisVol;
				soundIndex = (soundIndex + 1) % soundBuffLeng;
				if (soundIndex == maxIndex)
				{
					j = soundBuffLeng;
					maxVol = 0;
					while (j-- > 0)
					{
						if (soundBuff[j] > maxVol)
						{
							maxVol = soundBuff[j];
							maxIndex = j;
						}
					}
				}

				thisVol = (lastVol * 99 + maxVol) / 100;

				j = this->nChannels;
				while (j-- > 0)
				{
					if (thisVol > noiseVol)
					{
						currSample = Double2Int32(*(Int16*)&blk[i + j * 2] * 32767 / (thisVol * log32768 / Math_Log10(thisVol)));
					}
					else
					{
						currSample = Double2Int32(*(Int16*)&blk[i + j * 2] * 32767 / (noiseVol * log32768 / log32));
					}
					if (currSample > 32767)
					{
						currSample = 32767;
					}
					else if (currSample <= -32768)
					{
						currSample = -32768;
					}
					*(Int16*)&blk[i + j * 2] = (Int16)currSample;
				}
				lastVol = thisVol;

				i += this->nChannels << 1;
			}
			mutUsage.EndUse();
		}
		else if (this->bitCount == 8)
		{
			Sync::MutexUsage mutUsage(this->mut);
			Double log32 = Math_Log10(this->bgLevel * 128);
			Double log32768 = Math_Log10(128);
			Int32 noiseVol = Double2Int32(this->bgLevel * 128);
			UOSInt i = 0;
			UOSInt j;
			Int32 currSample;
			Int32 absVol;
			Int32 thisVol;
			while (i < readSize)
			{
				thisVol = 0;
				j = this->nChannels;
				while (j-- > 0)
				{
					currSample = blk[i + j] - 0x80;
					if (currSample < 0)
					{
						absVol = -(Int32)currSample;
					}
					else
					{
						absVol = currSample;
					}
					if (absVol > thisVol)
						thisVol = absVol;
				}
				if (thisVol >= this->maxVol)
				{
					maxVol = thisVol;
					maxIndex = soundIndex;
				}
				soundBuff[soundIndex] = thisVol;
				soundIndex = (soundIndex + 1) % soundBuffLeng;
				if (soundIndex == maxIndex)
				{
					j = soundBuffLeng;
					maxVol = 0;
					while (j-- > 0)
					{
						if (soundBuff[j] > maxVol)
						{
							maxVol = soundBuff[j];
							maxIndex = j;
						}
					}
				}

				thisVol = (lastVol * 99 + maxVol) / 100;

				j = this->nChannels;
				while (j-- > 0)
				{
					if (thisVol > noiseVol)
					{
						currSample = Double2Int32((blk[i + j] - 128) * 127 / (thisVol * log32768 / Math_Log10(thisVol)));
					}
					else
					{
						currSample = Double2Int32((blk[i + j] - 128) * 127 / (noiseVol * log32768 / log32));
					}
					if (currSample > 127)
					{
						currSample = 127;
					}
					else if (currSample <= -128)
					{
						currSample = -128;
					}
					blk[i + j] = (UInt8)(currSample + 128);
				}
				lastVol = thisVol;

				i += this->nChannels;
			}
			mutUsage.EndUse();
		}
	}
	return readSize;
}

void Media::AudioFilter::DynamicVolBooster::SetEnabled(Bool enabled)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->enabled = enabled;
	if (this->enabled)
		this->ResetStatus();
	mutUsage.EndUse();
}

void Media::AudioFilter::DynamicVolBooster::SetBGLevel(Double bgLevel)
{
	if (bgLevel > 0)
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->bgLevel = bgLevel;
		mutUsage.EndUse();
	}
}
