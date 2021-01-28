#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/AudioFilter/AudioSweepFilter.h"
#include "Text/MyString.h"

Media::AudioFilter::AudioSweepFilter::AudioSweepFilter(IAudioSource *sourceAudio) : Media::IAudioFilter(sourceAudio)
{
	this->sourceAudio = sourceAudio;
	sourceAudio->GetFormat(&this->format);

	NEW_CLASS(this->mut, Sync::Mutex());
	this->SetVolume(1.0);
	this->currSample = 0;
	this->endSample = 0;
	this->startFreq = 0;
	this->endFreq = 0;
}

Media::AudioFilter::AudioSweepFilter::~AudioSweepFilter()
{
	SDEL_CLASS(this->mut);
}

void Media::AudioFilter::AudioSweepFilter::GetFormat(AudioFormat *format)
{
	format->FromAudioFormat(&this->format);
}

UOSInt Media::AudioFilter::AudioSweepFilter::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	if (this->sourceAudio == 0)
		return 0;
	UOSInt readSize = this->sourceAudio->ReadBlock(buff, blkSize);

	Double startFreq;
	Double endFreq;
	UInt32 currSample;
	UInt32 endSample;
	this->mut->Lock();
	startFreq = this->startFreq;
	endFreq = this->endFreq;
	currSample = this->currSample;
	endSample = this->endSample;
	this->mut->Unlock();

	if (currSample >= endSample)
		return readSize;

	if (this->format.bitpersample == 16)
	{

		UOSInt i = 0;
		UOSInt j = readSize;
		OSInt k;
		OSInt nSamples = readSize / (this->format.nChannels << 1);
		Int32 pos = currSample;
		Double freq;
		Double t;
		Double v;
		Int32 iv;
		Int32 ivCh;
		t = this->currT;
		if (currSample + nSamples > endSample)
		{
			nSamples = endSample - currSample;
			j = nSamples * (this->format.nChannels << 1);
		}
		while (i < j)
		{
			freq = startFreq + (endFreq - startFreq) * (Math::Pow(10, (1 + pos) / (Double)endSample) - 1) / 9;
			t += freq / this->format.frequency * 2 * Math::PI;
			v = Math::Sin(t);

			iv = Math::Double2Int32(v * 32767 * this->vol);
			k = this->format.nChannels;
			while (k-- > 0)
			{
				ivCh = iv + ReadInt16(&buff[i]);
				if (ivCh > 32767)
					ivCh = 32767;
				if (ivCh < -32768)
					ivCh = -32768;
				WriteInt16(&buff[i], ivCh);
				i += 2;
			}
			pos++;
		}
		this->currSample += (Int32)nSamples;
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

Bool Media::AudioFilter::AudioSweepFilter::StartSweep(Double startFreq, Double endFreq, Int32 timeSeconds)
{
	this->mut->Lock();
	this->startFreq = startFreq;
	this->endFreq = endFreq;
	this->currSample = 0;
	this->currT = 0;
	this->endSample = this->format.frequency * timeSeconds;
	this->mut->Unlock();
	return true;
}
