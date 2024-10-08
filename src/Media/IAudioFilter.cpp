#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/IAudioFilter.h"
#include "Text/MyString.h"

Media::IAudioFilter::IAudioFilter(NN<IAudioSource> sourceAudio)
{
	this->sourceAudio = sourceAudio;
}

Media::IAudioFilter::~IAudioFilter()
{
}

UnsafeArrayOpt<UTF8Char> Media::IAudioFilter::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return this->sourceAudio->GetSourceName(buff);
}

Bool Media::IAudioFilter::CanSeek()
{
	return this->sourceAudio->CanSeek();
}

Data::Duration Media::IAudioFilter::GetStreamTime()
{
	return this->sourceAudio->GetStreamTime();
}

Data::Duration Media::IAudioFilter::SeekToTime(Data::Duration time)
{
	return this->sourceAudio->SeekToTime(time);
}

Bool Media::IAudioFilter::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	return this->sourceAudio->TrimStream(trimTimeStart, trimTimeEnd, syncTime);
}

void Media::IAudioFilter::GetFormat(NN<AudioFormat> format)
{
	this->sourceAudio->GetFormat(format);
}

Bool Media::IAudioFilter::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->sourceAudio->Start(evt, blkSize);
	return true;
}

void Media::IAudioFilter::Stop()
{
	this->sourceAudio->Stop();
}

UOSInt Media::IAudioFilter::GetMinBlockSize()
{
	return this->sourceAudio->GetMinBlockSize();
}

Data::Duration Media::IAudioFilter::GetCurrTime()
{
	return this->sourceAudio->GetCurrTime();
}

Bool Media::IAudioFilter::IsEnd()
{
	return this->sourceAudio->IsEnd();
}
