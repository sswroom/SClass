#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Decoder/VDecoderChain.h"

void Media::Decoder::VDecoderChain::ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	this->frameCb(frameTime, frameNum, imgData, dataSize, frameStruct, this->frameCbData, frameType, flags, ycOfst);
}

Media::Decoder::VDecoderChain::VDecoderChain(NotNullPtr<IVideoSource> sourceVideo) : Media::Decoder::VDecoderBase(sourceVideo)
{
}

Media::Decoder::VDecoderChain::~VDecoderChain()
{
	if (this->sourceVideo)
	{
		DEL_CLASS(this->sourceVideo);
	}
	UOSInt i = this->srcFilters.GetCount();
	while (i-- > 0)
	{
		Media::IVideoSource *video = this->srcFilters.GetItem(i);
		DEL_CLASS(video);
	}
}

Text::CStringNN Media::Decoder::VDecoderChain::GetFilterName()
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFilterName();
	}
	return CSTR("VDecoderChain");
}

void Media::Decoder::VDecoderChain::AddDecoder(NotNullPtr<Media::IVideoSource> decoder)
{
	this->srcFilters.Add(decoder.Ptr());
}

Bool Media::Decoder::VDecoderChain::CaptureImage(ImageCallback imgCb, AnyType userData)
{
	return this->sourceVideo->CaptureImage(imgCb, userData);
}

Bool Media::Decoder::VDecoderChain::HasFrameCount()
{
	return this->sourceVideo->HasFrameCount();
}

UOSInt Media::Decoder::VDecoderChain::GetFrameCount()
{
	return this->sourceVideo->GetFrameCount();
}

Data::Duration Media::Decoder::VDecoderChain::GetFrameTime(UOSInt frameIndex)
{
	return this->sourceVideo->GetFrameTime(frameIndex);
}

void Media::Decoder::VDecoderChain::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
	this->sourceVideo->EnumFrameInfos(cb, userData);
}

Bool Media::Decoder::VDecoderChain::GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	return this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
}
