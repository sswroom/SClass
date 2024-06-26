#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VideoFilter/VideoFilterBase.h"

void __stdcall Media::VideoFilter::VideoFilterBase::OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	NN<Media::VideoFilter::VideoFilterBase> me = userData.GetNN<Media::VideoFilter::VideoFilterBase>();
	me->ProcessVideoFrame(frameTime, frameNum, imgData, dataSize, frameStruct, userData, frameType, flags, ycOfst);
}

void __stdcall Media::VideoFilter::VideoFilterBase::OnVideoChange(Media::IVideoSource::FrameChange fc, AnyType userData)
{
	NN<Media::VideoFilter::VideoFilterBase> me = userData.GetNN<Media::VideoFilter::VideoFilterBase>();
	me->OnFrameChange(fc);
	if (me->fcCb)
	{
		me->fcCb(fc, me->userData);
	}
}

void Media::VideoFilter::VideoFilterBase::OnFrameChange(Media::IVideoSource::FrameChange fc)
{
}

Media::VideoFilter::VideoFilterBase::VideoFilterBase(Media::IVideoSource *srcVideo)
{
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UOSInt maxFrameSize;
	this->srcVideo = srcVideo;
	this->fcCb = 0;
	this->videoCb = 0;
	this->userData = 0;

	if (this->srcVideo)
	{
		this->srcVideo->GetVideoInfo(this->videoInfo, frameRateNorm, frameRateDenorm, maxFrameSize);
	}
}

Media::VideoFilter::VideoFilterBase::~VideoFilterBase()
{
}

void Media::VideoFilter::VideoFilterBase::SetSourceVideo(Media::IVideoSource *srcVideo)
{
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UOSInt maxFrameSize;
	this->srcVideo = srcVideo;
	if (this->srcVideo)
	{
		this->srcVideo->GetVideoInfo(this->videoInfo, frameRateNorm, frameRateDenorm, maxFrameSize);
	}
	this->OnFrameChange(Media::IVideoSource::FC_SRCCHG);
	if (this->fcCb)
	{
		this->fcCb(Media::IVideoSource::FC_SRCCHG, this->userData);
	}
}

UnsafeArrayOpt<UTF8Char> Media::VideoFilter::VideoFilterBase::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	if (this->srcVideo)
		return this->srcVideo->GetSourceName(buff);
	return 0;
}

void Media::VideoFilter::VideoFilterBase::SetBorderCrop(UOSInt cropLeft, UOSInt cropTop, UOSInt cropRight, UOSInt cropBottom)
{
	if (this->srcVideo)
		this->srcVideo->SetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
}

void Media::VideoFilter::VideoFilterBase::GetBorderCrop(OutParam<UOSInt> cropLeft, OutParam<UOSInt> cropTop, OutParam<UOSInt> cropRight, OutParam<UOSInt> cropBottom)
{
	if (this->srcVideo)
		this->srcVideo->GetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
	else
	{
		cropLeft.Set(0);
		cropTop.Set(0);
		cropRight.Set(0);
		cropBottom.Set(0);
	}
}

Bool Media::VideoFilter::VideoFilterBase::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	if (this->srcVideo)
	{
		Bool succ = this->srcVideo->GetVideoInfo(this->videoInfo, frameRateNorm, frameRateDenorm, maxFrameSize);
		if (succ)
		{
			info->Set(this->videoInfo);
		}
		return succ;
	}
	return false;
}

Bool Media::VideoFilter::VideoFilterBase::Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData)
{
	if (this->srcVideo)
	{
		this->srcVideo->Init(OnVideoFrame, OnVideoChange, this);
	}
	this->videoCb = cb;
	this->fcCb = fcCb;
	this->userData = userData;
	return this->srcVideo != 0;
}

Bool Media::VideoFilter::VideoFilterBase::Start()
{
	if (this->srcVideo)
		return this->srcVideo->Start();
	return false;
}

void Media::VideoFilter::VideoFilterBase::Stop()
{
	if (this->srcVideo)
	{
		this->srcVideo->Stop();
	}
}

Bool Media::VideoFilter::VideoFilterBase::IsRunning()
{
	if (this->srcVideo)
	{
		return this->srcVideo->IsRunning();
	}
	return false;
}

Data::Duration Media::VideoFilter::VideoFilterBase::GetStreamTime()
{
	if (this->srcVideo)
	{
		return this->srcVideo->GetStreamTime();
	}
	return 0;
}

Bool Media::VideoFilter::VideoFilterBase::CanSeek()
{
	if (this->srcVideo)
	{
		return this->srcVideo->CanSeek();
	}
	return false;
}

Data::Duration Media::VideoFilter::VideoFilterBase::SeekToTime(Data::Duration time)
{
	if (this->srcVideo)
	{
		return this->srcVideo->SeekToTime(time);
	}
	return false;
}

Bool Media::VideoFilter::VideoFilterBase::IsRealTimeSrc()
{
	if (this->srcVideo)
	{
		return this->srcVideo->IsRealTimeSrc();
	}
	return false;
}

Bool Media::VideoFilter::VideoFilterBase::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	if (this->srcVideo)
	{
		return this->srcVideo->TrimStream(trimTimeStart, trimTimeEnd, syncTime);
	}
	return false;
}

UOSInt Media::VideoFilter::VideoFilterBase::GetDataSeekCount()
{
	if (this->srcVideo)
	{
		return this->srcVideo->GetDataSeekCount();
	}
	return 0;
}

Bool Media::VideoFilter::VideoFilterBase::HasFrameCount()
{
	if (this->srcVideo)
	{
		return this->srcVideo->HasFrameCount();
	}
	return false;
}

UOSInt Media::VideoFilter::VideoFilterBase::GetFrameCount()
{
	if (this->srcVideo)
	{
		return this->srcVideo->GetFrameCount();
	}
	return 0;
}

Data::Duration Media::VideoFilter::VideoFilterBase::GetFrameTime(UOSInt frameIndex)
{
	if (this->srcVideo)
	{
		return this->srcVideo->GetFrameTime(frameIndex);
	}
	return 0;
}

void Media::VideoFilter::VideoFilterBase::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
	if (this->srcVideo)
	{
		this->srcVideo->EnumFrameInfos(cb, userData);
	}
}

UOSInt Media::VideoFilter::VideoFilterBase::ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype)
{
	return 0;
}
