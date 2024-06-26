#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Decoder/VDecoderBase.h"

void __stdcall Media::Decoder::VDecoderBase::OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	NN<Media::Decoder::VDecoderBase> me = userData.GetNN<Media::Decoder::VDecoderBase>();
	me->ProcVideoFrame(frameTime, frameNum, imgData, dataSize, frameStruct, frameType, flags, ycOfst);
}

void __stdcall Media::Decoder::VDecoderBase::OnVideoChange(Media::IVideoSource::FrameChange fc, AnyType userData)
{
	NN<Media::Decoder::VDecoderBase> me = userData.GetNN<Media::Decoder::VDecoderBase>();
	me->OnFrameChanged(fc);
	me->fcCb(fc, me->frameCbData);
}

Media::Decoder::VDecoderBase::VDecoderBase(NN<IVideoSource> sourceVideo)
{
	this->sourceVideo = sourceVideo.Ptr();
	this->fcCb = 0;
	this->started = false;
}

Media::Decoder::VDecoderBase::~VDecoderBase()
{
}

void Media::Decoder::VDecoderBase::SetBorderCrop(UOSInt cropLeft, UOSInt cropTop, UOSInt cropRight, UOSInt cropBottom)
{
	this->sourceVideo->SetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
}

void Media::Decoder::VDecoderBase::GetBorderCrop(OutParam<UOSInt> cropLeft, OutParam<UOSInt> cropTop, OutParam<UOSInt> cropRight, OutParam<UOSInt> cropBottom)
{
	this->sourceVideo->GetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
}

UnsafeArrayOpt<UTF8Char> Media::Decoder::VDecoderBase::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	if (this->sourceVideo)
		return this->sourceVideo->GetSourceName(buff);
	//////////////////////////////////////
	return 0;
}

Bool Media::Decoder::VDecoderBase::Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData)
{
	if (this->sourceVideo == 0)
		return false;
	this->frameCb = cb;
	this->fcCb = fcCb;
	this->frameCbData = userData;
	this->sourceVideo->Init(OnVideoFrame, OnVideoChange, this);
	return true;
}

Bool Media::Decoder::VDecoderBase::Start()
{
	if (this->sourceVideo == 0)
		return false;

	this->started = true;
	return this->sourceVideo->Start();
}

void Media::Decoder::VDecoderBase::Stop()
{
	if (this->sourceVideo == 0)
		return;

	this->started = false;
	this->sourceVideo->Stop();
	this->frameCb = 0;
	this->frameCbData = 0;
}

Bool Media::Decoder::VDecoderBase::IsRunning()
{
	if (this->sourceVideo == 0)
		return false;
	return this->sourceVideo->IsRunning();
}

Data::Duration Media::Decoder::VDecoderBase::GetStreamTime()
{
	if (this->sourceVideo)
		return this->sourceVideo->GetStreamTime();
	return 0;
}

Bool Media::Decoder::VDecoderBase::CanSeek()
{
	if (this->sourceVideo)
		return this->sourceVideo->CanSeek();
	return false;
}

Data::Duration Media::Decoder::VDecoderBase::SeekToTime(Data::Duration time)
{
	if (this->sourceVideo)
		return this->sourceVideo->SeekToTime(time);
	return 0;
}

Bool Media::Decoder::VDecoderBase::IsRealTimeSrc()
{
	if (this->sourceVideo)
		return this->sourceVideo->IsRealTimeSrc();
	return false;
}

Bool Media::Decoder::VDecoderBase::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	if (this->sourceVideo)
		return this->sourceVideo->TrimStream(trimTimeStart, trimTimeEnd, syncTime);
	return false;
}

UOSInt Media::Decoder::VDecoderBase::GetDataSeekCount()
{
	if (this->sourceVideo)
		return this->sourceVideo->GetDataSeekCount();
	return 0;
}

UOSInt Media::Decoder::VDecoderBase::ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype)
{
	return 0;
}

UInt8 *Media::Decoder::VDecoderBase::GetProp(Int32 propName, UInt32 *size)
{
	UInt8 *ret = Media::IVideoSource::GetProp(propName, size);
	if (ret)
		return ret;
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetProp(propName, size);
	}
	return 0;
}

void Media::Decoder::VDecoderBase::OnFrameChanged(Media::IVideoSource::FrameChange fc)
{
}
