#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/AudioFilter/AudioSweepFilter.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Media::AudioFilter::AudioSweepFilter::AudioSweepFilter(NotNullPtr<IAudioSource> sourceAudio) : Media::IAudioFilter(sourceAudio)
{
	this->sourceAudio = sourceAudio;
	sourceAudio->GetFormat(this->format);

	this->SetVolume(1.0);
	this->currSample = 0;
	this->endSample = 0;
	this->startFreq = 0;
	this->endFreq = 0;
}

Media::AudioFilter::AudioSweepFilter::~AudioSweepFilter()
{
}

void Media::AudioFilter::AudioSweepFilter::GetFormat(NotNullPtr<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

UOSInt Media::AudioFilter::AudioSweepFilter::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);

	Double startFreq;
	Double endFreq;
	UInt32 currSample;
	UInt32 endSample;
	Sync::MutexUsage mutUsage(this->mut);
	startFreq = this->startFreq;
	endFreq = this->endFreq;
	currSample = this->currSample;
	endSample = this->endSample;
	mutUsage.EndUse();

	if (currSample >= endSample)
		return readSize;

	if (this->format.bitpersample == 16)
	{

		UOSInt i = 0;
		UOSInt j = readSize;
		OSInt k;
		UOSInt nSamples = readSize / ((UOSInt)this->format.nChannels << 1);
		UInt32 pos = currSample;
		Double freq;
		Double t;
		Double v;
		Int32 iv;
		Int32 ivCh;
		t = this->currT;
		if (currSample + nSamples > endSample)
		{
			nSamples = endSample - currSample;
			j = nSamples * ((UOSInt)this->format.nChannels << 1);
		}
		while (i < j)
		{
			freq = startFreq + (endFreq - startFreq) * (Math_Pow(10, (1 + pos) / (Double)endSample) - 1) / 9;
			t += freq / this->format.frequency * 2 * Math::PI;
			v = Math_Sin(t);

			iv = Double2Int32(v * 32767 * this->vol);
			k = this->format.nChannels;
			while (k-- > 0)
			{
				ivCh = iv + ReadInt16(&blk[i]);
				if (ivCh > 32767)
					ivCh = 32767;
				if (ivCh < -32768)
					ivCh = -32768;
				WriteInt16(&blk[i], (Int16)ivCh);
				i += 2;
			}
			pos++;
		}
		this->currSample += (UInt32)nSamples;
		this->currT = t;
	}
	else if (this->format.bitpersample == 8)
	{
	}
	return readSize;
}

void Media::AudioFilter::AudioSweepFilter::SetVolume(Double vol)
{
	this->vol = vol;
}

Bool Media::AudioFilter::AudioSweepFilter::StartSweep(Double startFreq, Double endFreq, UInt32 timeSeconds)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->startFreq = startFreq;
	this->endFreq = endFreq;
	this->currSample = 0;
	this->currT = 0;
	this->endSample = this->format.frequency * timeSeconds;
	mutUsage.EndUse();
	return true;
}
