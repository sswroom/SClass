#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/AudioUtil.h"
#include "Media/AFilter/AudioAmplifier.h"

Media::AFilter::AudioAmplifier::AudioAmplifier(NN<Media::AudioSource> sourceAudio) : Media::AudioFilter(sourceAudio)
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

Media::AFilter::AudioAmplifier::~AudioAmplifier()
{
}

void Media::AFilter::AudioAmplifier::GetFormat(NN<AudioFormat> format)
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

Data::Duration Media::AFilter::AudioAmplifier::SeekToTime(Data::Duration time)
{
	return this->sourceAudio->SeekToTime(time);
}

UOSInt Media::AFilter::AudioAmplifier::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);
	Double thisVol = this->level;
	if (thisVol != 1.0)
	{
		Int32 iVol = Double2Int32(thisVol * 65536.0);
		if (this->bitCount == 16)
		{
			AudioUtil_Amplify16(blk.Arr().Ptr(), readSize, iVol);
		}
		else if (this->bitCount == 8)
		{
			AudioUtil_Amplify8(blk.Arr().Ptr(), readSize, iVol);
		}
	}
	return readSize;
}

void Media::AFilter::AudioAmplifier::SetLevel(Double level)
{
	if (level >= 0)
	{
		this->level = level;
	}
}
