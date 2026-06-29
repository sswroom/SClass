#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Decoder/VDecoderBase.h"

void __stdcall Media::Decoder::VDecoderBase::OnVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	NN<Media::Decoder::VDecoderBase> me = userData.GetNN<Media::Decoder::VDecoderBase>();
	me->ProcVideoFrame(frameTime, frameNum, imgData, dataSize, frameStruct, frameType, flags, ycOfst);
}

void __stdcall Media::Decoder::VDecoderBase::OnVideoChange(Media::VideoSource::FrameChange fc, AnyType userData)
{
	NN<Media::Decoder::VDecoderBase> me = userData.GetNN<Media::Decoder::VDecoderBase>();
	me->OnFrameChanged(fc);
	me->fcCb(fc, me->frameCbData);
}

Media::Decoder::VDecoderBase::VDecoderBase(NN<VideoSource> sourceVideo)
{
	this->sourceVideo = sourceVideo;
	this->fcCb = 0;
	this->started = false;
}

Media::Decoder::VDecoderBase::~VDecoderBase()
{
}

void Media::Decoder::VDecoderBase::SetBorderCrop(UIntOS cropLeft, UIntOS cropTop, UIntOS cropRight, UIntOS cropBottom)
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return;
	sourceVideo->SetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
}

void Media::Decoder::VDecoderBase::GetBorderCrop(OutParam<UIntOS> cropLeft, OutParam<UIntOS> cropTop, OutParam<UIntOS> cropRight, OutParam<UIntOS> cropBottom)
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return;
	sourceVideo->GetBorderCrop(cropLeft, cropTop, cropRight, cropBottom);
}

UnsafeArrayOpt<UTF8Char> Media::Decoder::VDecoderBase::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	NN<Media::VideoSource> sourceVideo;
	if (this->sourceVideo.SetTo(sourceVideo))
		return sourceVideo->GetSourceName(buff);
	//////////////////////////////////////
	return nullptr;
}

Bool Media::Decoder::VDecoderBase::Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData)
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return false;
	this->frameCb = cb;
	this->fcCb = fcCb;
	this->frameCbData = userData;
	sourceVideo->Init(OnVideoFrame, OnVideoChange, this);
	return true;
}

Bool Media::Decoder::VDecoderBase::Start()
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return false;

	this->started = true;
	return sourceVideo->Start();
}

void Media::Decoder::VDecoderBase::Stop()
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return;

	this->started = false;
	sourceVideo->Stop();
	this->frameCb = 0;
	this->frameCbData = 0;
}

Bool Media::Decoder::VDecoderBase::IsRunning()
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return false;
	return sourceVideo->IsRunning();
}

Data::Duration Media::Decoder::VDecoderBase::GetStreamTime()
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return 0;
	return sourceVideo->GetStreamTime();
}

Bool Media::Decoder::VDecoderBase::CanSeek()
{
//	if (this->sourceVideo)
//		return this->sourceVideo->CanSeek();
	return false;
}

Data::Duration Media::Decoder::VDecoderBase::SeekToTime(Data::Duration time)
{
//	if (this->sourceVideo)
//		return this->sourceVideo->SeekToTime(time);
	return 0;
}

Bool Media::Decoder::VDecoderBase::IsRealTimeSrc()
{
//	if (this->sourceVideo)
//		return this->sourceVideo->IsRealTimeSrc();
	return false;
}

Bool Media::Decoder::VDecoderBase::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
//	if (this->sourceVideo)
//		return this->sourceVideo->TrimStream(trimTimeStart, trimTimeEnd, syncTime);
	return false;
}

UIntOS Media::Decoder::VDecoderBase::GetDataSeekCount()
{
//	if (this->sourceVideo)
//		return this->sourceVideo->GetDataSeekCount();
	return 0;
}

UIntOS Media::Decoder::VDecoderBase::ReadNextFrame(UnsafeArray<UInt8> frameBuff, OutParam<UInt32> frameTime, OutParam<Media::FrameType> ftype)
{
	return 0;
}

UInt8 *Media::Decoder::VDecoderBase::GetProp(Int32 propName, UInt32 *size)
{
	UInt8 *ret = Media::VideoSource::GetProp(propName, size);
	if (ret)
		return ret;
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return nullptr;
	return sourceVideo->GetProp(propName, size);
}

void Media::Decoder::VDecoderBase::OnFrameChanged(Media::VideoSource::FrameChange fc)
{
}
