#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/IAudioFilter.h"
#include "Text/MyString.h"

Media::IAudioFilter::IAudioFilter(IAudioSource *sourceAudio)
{
	this->sourceAudio = sourceAudio;
}

Media::IAudioFilter::~IAudioFilter()
{
}

UTF8Char *Media::IAudioFilter::GetSourceName(UTF8Char *buff)
{
	if (this->sourceAudio)
        return this->sourceAudio->GetSourceName(buff);
	return 0;
}

Bool Media::IAudioFilter::CanSeek()
{
	if (this->sourceAudio)
		return this->sourceAudio->CanSeek();
	return false;
}

Int32 Media::IAudioFilter::GetStreamTime()
{
	if (this->sourceAudio)
		return this->sourceAudio->GetStreamTime();
	return 0;
}

UInt32 Media::IAudioFilter::SeekToTime(UInt32 time)
{
	if (this->sourceAudio)
	{
		return this->sourceAudio->SeekToTime(time);
	}
	return 0;
}

Bool Media::IAudioFilter::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	if (this->sourceAudio)
	{
		return this->sourceAudio->TrimStream(trimTimeStart, trimTimeEnd, syncTime);
	}
	return true;
}

void Media::IAudioFilter::GetFormat(AudioFormat *format)
{
	if (this->sourceAudio)
	{
		this->sourceAudio->GetFormat(format);
	}
	else
	{
		format->Clear();
	}
}

Bool Media::IAudioFilter::Start(Sync::Event *evt, UOSInt blkSize)
{
	if (this->sourceAudio)
	{
		this->sourceAudio->Start(evt, blkSize);
		return true;
	}
	return false;
}

void Media::IAudioFilter::Stop()
{
	if (this->sourceAudio)
	{
		this->sourceAudio->Stop();
	}
}

UOSInt Media::IAudioFilter::GetMinBlockSize()
{
	if (this->sourceAudio)
		return this->sourceAudio->GetMinBlockSize();
	return 0;
}

UInt32 Media::IAudioFilter::GetCurrTime()
{
	if (this->sourceAudio)
		return this->sourceAudio->GetCurrTime();
	return 0;
}

Bool Media::IAudioFilter::IsEnd()
{
	if (this->sourceAudio)
		return this->sourceAudio->IsEnd();
	return true;
}
