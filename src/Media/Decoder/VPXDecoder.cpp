#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Media/StaticImage.h"
#include "Media/Decoder/VPXDecoder.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "vpx/vp8dx.h"
#include "vpx/vpx_decoder.h"

//#include "tools_common.h"

struct Media::Decoder::VPXDecoder::ClassData
{
	Bool inited;
	vpx_codec_ctx_t codec;
};

void Media::Decoder::VPXDecoder::ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
}

Media::Decoder::VPXDecoder::VPXDecoder(NN<VideoSource> sourceVideo) : Media::Decoder::VDecoderBase(sourceVideo)
{
	NN<ClassData> data = MemAllocNN(ClassData);
	this->clsData = data;
	data->inited = false;

	Media::FrameInfo frameInfo;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UIntOS maxFrameSize;
	sourceVideo->GetVideoInfo(frameInfo, frameRateNorm, frameRateDenorm, maxFrameSize);

/*	vpx_codec_iface_t *(*codec_interface)();
	if (frameInfo.fourcc == *(Int32*)"VP80")
	{
		codec_interface = &vpx_codec_vp8_dx;
	}
	else if (frameInfo.fourcc == *(Int32*)"VP90")
	{
		codec_interface = &vpx_codec_vp9_dx;
	}
	else
	{
		return;
	}
	if (vpx_codec_dec_init(&data->codec, codec_interface(), 0, 0))
	{
		return;
	}
	data->inited = true;*/
}

Media::Decoder::VPXDecoder::~VPXDecoder()
{
	NN<ClassData> data = this->clsData;
/*	if (data->inited)
	{
		vpx_codec_destroy(&data->codec);
		data->inited = false;
	}*/
	MemFreeNN(data);
}

Bool Media::Decoder::VPXDecoder::CaptureImage(ImageCallback imgCb, AnyType userData)
{
	return false;
}

Text::CStringNN Media::Decoder::VPXDecoder::GetFilterName()
{
	return CSTR("VPXDecoder");
}

Bool Media::Decoder::VPXDecoder::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize)
{
	return false;
}

void Media::Decoder::VPXDecoder::Stop()
{
}

UIntOS Media::Decoder::VPXDecoder::GetFrameCount()
{
	return 0;
}

Data::Duration Media::Decoder::VPXDecoder::GetFrameTime(UIntOS frameIndex) const
{
	return 0;
}

void Media::Decoder::VPXDecoder::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
}

void Media::Decoder::VPXDecoder::OnFrameChanged(Media::VideoSource::FrameChange fc)
{
}

Bool Media::Decoder::VPXDecoder::IsError()
{
	return !this->clsData->inited;
}

Optional<Media::VideoSource> __stdcall VPXDecoder_DecodeVideo(NN<Media::VideoSource> sourceVideo)
{
	NN<Media::Decoder::VPXDecoder> decoder;
	Media::FrameInfo frameInfo;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UIntOS maxFrameSize;
	sourceVideo->GetVideoInfo(frameInfo, frameRateNorm, frameRateDenorm, maxFrameSize);
	if (frameInfo.fourcc == 0 || frameInfo.fourcc == -1)
		return nullptr;

	if (frameInfo.fourcc == *(Int32*)"VP90" || frameInfo.fourcc == *(Int32*)"VP80")
	{
		NEW_CLASSNN(decoder, Media::Decoder::VPXDecoder(sourceVideo));
		if (decoder->IsError())
		{
			decoder.Delete();
			return nullptr;
		}
		return decoder;
	}
	return nullptr;
}

void Media::Decoder::VPXDecoder::Enable()
{
	Core::CoreAddVideoDecFunc(VPXDecoder_DecodeVideo);
}
