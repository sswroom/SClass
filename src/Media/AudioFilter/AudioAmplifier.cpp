#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/AudioUtil.h"
#include "Media/AudioFilter/AudioAmplifier.h"

Media::AudioFilter::AudioAmplifier::AudioAmplifier(NotNullPtr<Media::IAudioSource> sourceAudio) : Media::IAudioFilter(sourceAudio)
{
	Media::AudioFormat fmt;
	this->level = 1.0;
	this->bitCount = 0;
	sourceAudio->GetFormat(fmt);
	if (fmt.formatId != 0x1)
		return;
	if (fmt.bitpersample != 16 && fmt.bitpersample != 8)
		return;
	this->bitCount = fmt.bitpersample;
}

Media::AudioFilter::AudioAmplifier::~AudioAmplifier()
{
}

void Media::AudioFilter::AudioAmplifier::GetFormat(NotNullPtr<AudioFormat> format)
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

UInt32 Media::AudioFilter::AudioAmplifier::SeekToTime(UInt32 time)
{
	return this->sourceAudio->SeekToTime(time);
}

UOSInt Media::AudioFilter::AudioAmplifier::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);
	Double thisVol = this->level;
	if (thisVol != 1.0)
	{
		Int32 iVol = Double2Int32(thisVol * 65536.0);
		if (this->bitCount == 16)
		{
			AudioUtil_Amplify16(blk.Ptr(), readSize, iVol);
		}
		else if (this->bitCount == 8)
		{
			AudioUtil_Amplify8(blk.Ptr(), readSize, iVol);
		}
	}
	return readSize;
}

void Media::AudioFilter::AudioAmplifier::SetLevel(Double level)
{
	if (level >= 0)
	{
		this->level = level;
	}
}
