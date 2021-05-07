#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Decoder/VDecoderChain.h"

void Media::Decoder::VDecoderChain::ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	this->frameCb(frameTime, frameNum, imgData, dataSize, frameStruct, this->frameCbData, frameType, flags, ycOfst);
}

Media::Decoder::VDecoderChain::VDecoderChain(IVideoSource *sourceVideo) : Media::Decoder::VDecoderBase(sourceVideo)
{
	NEW_CLASS(this->srcFilters, Data::ArrayList<Media::IVideoSource*>());
}

Media::Decoder::VDecoderChain::~VDecoderChain()
{
	if (this->sourceVideo)
	{
		DEL_CLASS(this->sourceVideo);
	}
	UOSInt i = this->srcFilters->GetCount();
	while (i-- > 0)
	{
		Media::IVideoSource *video = this->srcFilters->GetItem(i);
		DEL_CLASS(video);
	}
	DEL_CLASS(this->srcFilters);
}

const UTF8Char *Media::Decoder::VDecoderChain::GetFilterName()
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFilterName();
	}
	return (const UTF8Char*)"VDecoderChain";
}

void Media::Decoder::VDecoderChain::AddDecoder(Media::IVideoSource *decoder)
{
	this->srcFilters->Add(decoder);
}

Bool Media::Decoder::VDecoderChain::CaptureImage(ImageCallback imgCb, void *userData)
{
	return this->sourceVideo->CaptureImage(imgCb, userData);
}

OSInt Media::Decoder::VDecoderChain::GetFrameCount()
{
	return this->sourceVideo->GetFrameCount();
}

UInt32 Media::Decoder::VDecoderChain::GetFrameTime(UOSInt frameIndex)
{
	return this->sourceVideo->GetFrameTime(frameIndex);
}

void Media::Decoder::VDecoderChain::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
	this->sourceVideo->EnumFrameInfos(cb, userData);
}

Bool Media::Decoder::VDecoderChain::GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	return this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
}
