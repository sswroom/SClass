#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VideoFilter/VideoFilterBase.h"

void __stdcall Media::VideoFilter::VideoFilterBase::OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	NN<Media::VideoFilter::VideoFilterBase> me = userData.GetNN<Media::VideoFilter::VideoFilterBase>();
	me->ProcessVideoFrame(frameTime, frameNum, imgData, dataSize, frameStruct, userData, frameType, flags, ycOfst);
}

void __stdcall Media::VideoFilter::VideoFilterBase::OnVideoChange(Media::VideoSource::FrameChange fc, AnyType userData)
{
	NN<Media::VideoFilter::VideoFilterBase> me = userData.GetNN<Media::VideoFilter::VideoFilterBase>();
	me->OnFrameChange(fc);
	if (me->fcCb)
	{
		me->fcCb(fc, me->userData);
	}
}

void Media::VideoFilter::VideoFilterBase::OnFrameChange(Media::VideoSource::FrameChange fc)
{
}

Media::VideoFilter::VideoFilterBase::VideoFilterBase(Optional<Media::VideoSource> srcVideo)
{
	NN<Media::VideoSource> nnsrcVideo;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UIntOS maxFrameSize;
	this->srcVideo = srcVideo;
	this->fcCb = 0;
	this->videoCb = 0;
	this->userData = 0;

	if (this->srcVideo.SetTo(nnsrcVideo))
	{
		nnsrcVideo->GetVideoInfo(this->videoInfo, frameRateNorm, frameRateDenorm, maxFrameSize);
	}
}

Media::VideoFilter::VideoFilterBase::~VideoFilterBase()
{
}

void Media::VideoFilter::VideoFilterBase::SetSourceVideo(Optional<Media::VideoSource> srcVideo)
{
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UIntOS maxFrameSize;
	NN<Media::VideoSource> nnsrcVideo;
	this->srcVideo = srcVideo;
	if (this->srcVideo.SetTo(nnsrcVideo))
	{
		nnsrcVideo->GetVideoInfo(this->videoInfo, frameRateNorm, frameRateDenorm, maxFrameSize);
	}
	this->OnFrameChange(Media::VideoSource::FC_SRCCHG);
	if (this->fcCb)
	{
		this->fcCb(Media::VideoSource::FC_SRCCHG, this->userData);
	}
}

UnsafeArrayOpt<UTF8Char> Media::VideoFilter::VideoFilterBase::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
		return srcVideo->GetSourceName(buff);
	return nullptr;
}

void Media::VideoFilter::VideoFilterBase::SetBorderCrop(UIntOS cropLeft, UIntOS cropTop, UIntOS cropRight, UIntOS cropBottom)
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
		srcVideo->SetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
}

void Media::VideoFilter::VideoFilterBase::GetBorderCrop(OutParam<UIntOS> cropLeft, OutParam<UIntOS> cropTop, OutParam<UIntOS> cropRight, OutParam<UIntOS> cropBottom)
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
		srcVideo->GetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
	else
	{
		cropLeft.Set(0);
		cropTop.Set(0);
		cropRight.Set(0);
		cropBottom.Set(0);
	}
}

Bool Media::VideoFilter::VideoFilterBase::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize)
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		Bool succ = srcVideo->GetVideoInfo(this->videoInfo, frameRateNorm, frameRateDenorm, maxFrameSize);
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
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		srcVideo->Init(OnVideoFrame, OnVideoChange, this);
	}
	this->videoCb = cb;
	this->fcCb = fcCb;
	this->userData = userData;
	return this->srcVideo.NotNull();
}

Bool Media::VideoFilter::VideoFilterBase::Start()
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
		return srcVideo->Start();
	return false;
}

void Media::VideoFilter::VideoFilterBase::Stop()
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		srcVideo->Stop();
	}
}

Bool Media::VideoFilter::VideoFilterBase::IsRunning()
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		return srcVideo->IsRunning();
	}
	return false;
}

Data::Duration Media::VideoFilter::VideoFilterBase::GetStreamTime()
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		return srcVideo->GetStreamTime();
	}
	return 0;
}

Bool Media::VideoFilter::VideoFilterBase::CanSeek()
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		return srcVideo->CanSeek();
	}
	return false;
}

Data::Duration Media::VideoFilter::VideoFilterBase::SeekToTime(Data::Duration time)
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		return srcVideo->SeekToTime(time);
	}
	return 0;
}

Bool Media::VideoFilter::VideoFilterBase::IsRealTimeSrc()
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		return srcVideo->IsRealTimeSrc();
	}
	return false;
}

Bool Media::VideoFilter::VideoFilterBase::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		return srcVideo->TrimStream(trimTimeStart, trimTimeEnd, syncTime);
	}
	return false;
}

UIntOS Media::VideoFilter::VideoFilterBase::GetDataSeekCount()
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		return srcVideo->GetDataSeekCount();
	}
	return 0;
}

Bool Media::VideoFilter::VideoFilterBase::HasFrameCount()
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		return srcVideo->HasFrameCount();
	}
	return false;
}

UIntOS Media::VideoFilter::VideoFilterBase::GetFrameCount()
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		return srcVideo->GetFrameCount();
	}
	return 0;
}

Data::Duration Media::VideoFilter::VideoFilterBase::GetFrameTime(UIntOS frameIndex)
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		return srcVideo->GetFrameTime(frameIndex);
	}
	return 0;
}

void Media::VideoFilter::VideoFilterBase::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
	NN<Media::VideoSource> srcVideo;
	if (this->srcVideo.SetTo(srcVideo))
	{
		srcVideo->EnumFrameInfos(cb, userData);
	}
}

UIntOS Media::VideoFilter::VideoFilterBase::ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype)
{
	return 0;
}
