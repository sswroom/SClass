#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Math/Math_C.h"
#include "Media/AudioFilter.h"
#include "Text/MyString.h"

Media::AudioFilter::AudioFilter(NN<AudioSource> sourceAudio)
{
	this->sourceAudio = sourceAudio;
}

Media::AudioFilter::~AudioFilter()
{
}

UnsafeArrayOpt<UTF8Char> Media::AudioFilter::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return this->sourceAudio->GetSourceName(buff);
}

Bool Media::AudioFilter::CanSeek()
{
	return this->sourceAudio->CanSeek();
}

Data::Duration Media::AudioFilter::GetStreamTime()
{
	return this->sourceAudio->GetStreamTime();
}

Data::Duration Media::AudioFilter::SeekToTime(Data::Duration time)
{
	return this->sourceAudio->SeekToTime(time);
}

Bool Media::AudioFilter::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	return this->sourceAudio->TrimStream(trimTimeStart, trimTimeEnd, syncTime);
}

void Media::AudioFilter::GetFormat(NN<AudioFormat> format)
{
	this->sourceAudio->GetFormat(format);
}

Bool Media::AudioFilter::Start(Optional<Sync::Event> evt, UOSInt blkSize)
{
	this->sourceAudio->Start(evt, blkSize);
	return true;
}

void Media::AudioFilter::Stop()
{
	this->sourceAudio->Stop();
}

UOSInt Media::AudioFilter::GetMinBlockSize()
{
	return this->sourceAudio->GetMinBlockSize();
}

Data::Duration Media::AudioFilter::GetCurrTime()
{
	return this->sourceAudio->GetCurrTime();
}

Bool Media::AudioFilter::IsEnd()
{
	return this->sourceAudio->IsEnd();
}
