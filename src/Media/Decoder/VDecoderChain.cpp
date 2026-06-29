#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Decoder/VDecoderChain.h"

void Media::Decoder::VDecoderChain::ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	this->frameCb(frameTime, frameNum, imgData, dataSize, frameStruct, this->frameCbData, frameType, flags, ycOfst);
}

Media::Decoder::VDecoderChain::VDecoderChain(NN<VideoSource> sourceVideo) : Media::Decoder::VDecoderBase(sourceVideo)
{
}

Media::Decoder::VDecoderChain::~VDecoderChain()
{
	this->sourceVideo.Delete();
	this->srcFilters.DeleteAll();
}

Text::CStringNN Media::Decoder::VDecoderChain::GetFilterName()
{
	NN<Media::VideoSource> sourceVideo;
	if (this->sourceVideo.SetTo(sourceVideo))
	{
		return sourceVideo->GetFilterName();
	}
	return CSTR("VDecoderChain");
}

void Media::Decoder::VDecoderChain::AddDecoder(NN<Media::VideoSource> decoder)
{
	this->srcFilters.Add(decoder);
}

Bool Media::Decoder::VDecoderChain::CaptureImage(ImageCallback imgCb, AnyType userData)
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return false;
	return sourceVideo->CaptureImage(imgCb, userData);
}

Bool Media::Decoder::VDecoderChain::HasFrameCount()
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return false;
	return sourceVideo->HasFrameCount();
}

UIntOS Media::Decoder::VDecoderChain::GetFrameCount()
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return 0;
	return sourceVideo->GetFrameCount();
}

Data::Duration Media::Decoder::VDecoderChain::GetFrameTime(UIntOS frameIndex)
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return 0;
	return sourceVideo->GetFrameTime(frameIndex);
}

void Media::Decoder::VDecoderChain::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return;
	sourceVideo->EnumFrameInfos(cb, userData);
}

Bool Media::Decoder::VDecoderChain::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize)
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return false;
	return sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
}
