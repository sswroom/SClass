#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/IAudioFilter.h"
#include "Text/MyString.h"

Media::IAudioFilter::IAudioFilter(NotNullPtr<IAudioSource> sourceAudio)
{
	this->sourceAudio = sourceAudio;
}

Media::IAudioFilter::~IAudioFilter()
{
}

UTF8Char *Media::IAudioFilter::GetSourceName(UTF8Char *buff)
{
	return this->sourceAudio->GetSourceName(buff);
}

Bool Media::IAudioFilter::CanSeek()
{
	return this->sourceAudio->CanSeek();
}

Int32 Media::IAudioFilter::GetStreamTime()
{
	return this->sourceAudio->GetStreamTime();
}

UInt32 Media::IAudioFilter::SeekToTime(UInt32 time)
{
	return this->sourceAudio->SeekToTime(time);
}

Bool Media::IAudioFilter::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	return this->sourceAudio->TrimStream(trimTimeStart, trimTimeEnd, syncTime);
}

void Media::IAudioFilter::GetFormat(NotNullPtr<AudioFormat> format)
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

UInt32 Media::IAudioFilter::GetCurrTime()
{
	return this->sourceAudio->GetCurrTime();
}

Bool Media::IAudioFilter::IsEnd()
{
	return this->sourceAudio->IsEnd();
}
