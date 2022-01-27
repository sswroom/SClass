#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Media/StaticImage.h"
#include "Media/Decoder/VPXDecoder.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "vpx/vp8dx.h"
#include "vpx/vpx_decoder.h"

//#include "tools_common.h"

typedef struct
{
	Bool inited;
	vpx_codec_ctx_t codec;
} ClassData;

void Media::Decoder::VPXDecoder::ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
}

Media::Decoder::VPXDecoder::VPXDecoder(IVideoSource *sourceVideo) : Media::Decoder::VDecoderBase(sourceVideo)
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	data->inited = false;

	Media::FrameInfo frameInfo;
	Int32 frameRateNorm;
	Int32 frameRateDenorm;
	UOSInt maxFrameSize;
	sourceVideo->GetVideoInfo(&frameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize);

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
	ClassData *data = (ClassData*)this->clsData;
/*	if (data->inited)
	{
		vpx_codec_destroy(&data->codec);
		data->inited = false;
	}*/
	MemFree(data);
}

Bool Media::Decoder::VPXDecoder::CaptureImage(ImageCallback imgCb, void *userData)
{
	return false;
}

Text::CString Media::Decoder::VPXDecoder::GetFilterName()
{
	return CSTR("VPXDecoder");
}

Bool Media::Decoder::VPXDecoder::GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	return false;
}

void Media::Decoder::VPXDecoder::Stop()
{
}

OSInt Media::Decoder::VPXDecoder::GetFrameCount()
{
	return -1;
}

UInt32 Media::Decoder::VPXDecoder::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Media::Decoder::VPXDecoder::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
}

void Media::Decoder::VPXDecoder::OnFrameChanged(Media::IVideoSource::FrameChange fc)
{
}

Bool Media::Decoder::VPXDecoder::IsError()
{
	ClassData *data = (ClassData*)this->clsData;
	return !data->inited;
}

Media::IVideoSource *__stdcall VPXDecoder_DecodeVideo(Media::IVideoSource *sourceVideo)
{
	Media::Decoder::VPXDecoder *decoder;
	Media::FrameInfo frameInfo;
	Int32 frameRateNorm;
	Int32 frameRateDenorm;
	UOSInt maxFrameSize;
	sourceVideo->GetVideoInfo(&frameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize);
	if (frameInfo.fourcc == 0 || frameInfo.fourcc == -1)
		return 0;

	if (frameInfo.fourcc == *(Int32*)"VP90" || frameInfo.fourcc == *(Int32*)"VP80")
	{
		NEW_CLASS(decoder, Media::Decoder::VPXDecoder(sourceVideo));
		if (decoder->IsError())
		{
			DEL_CLASS(decoder);
			return 0;
		}
		return decoder;
	}
	return 0;
}

void Media::Decoder::VPXDecoder::Enable()
{
	Core::CoreAddVideoDecFunc(VPXDecoder_DecodeVideo);
}
