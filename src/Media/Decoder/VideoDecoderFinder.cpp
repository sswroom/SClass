#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
//#include "Media/Decoder/DSVDecoder.h"
#include "Media/Decoder/M2VDecoder.h"
#include "Media/Decoder/MP2GDecoder.h"
#include "Media/Decoder/RAVCDecoder.h"
#include "Media/Decoder/RHVCDecoder.h"
#include "Media/Decoder/VDecoderChain.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "Media/Decoder/VFWDecoder.h"
#include "Media/CS/CSConverter.h"

Media::Decoder::VideoDecoderFinder::VideoDecoderFinder()
{
}

Media::Decoder::VideoDecoderFinder::~VideoDecoderFinder()
{
}

Media::IVideoSource *Media::Decoder::VideoDecoderFinder::DecodeVideo(Media::IVideoSource *vsrc)
{
	Media::IVideoSource *decoder;
	Media::FrameInfo frameInfo;
	Media::FrameInfo decFrameInfo;
	Int32 frameRateNorm;
	Int32 frameRateDenorm;
	UOSInt maxFrameSize;
	vsrc->GetVideoInfo(&frameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize);
	if (frameInfo.fourcc == 0 || frameInfo.fourcc == 0xFFFFFFFF)
		return 0;

	Data::ArrayListInt32 suppCSList;
	Media::CS::CSConverter::GetSupportedCS(&suppCSList);
	if (suppCSList.IndexOf(frameInfo.fourcc) != (UOSInt)-1)
	{
		return 0;
	}
	decoder = Core::DecodeVideo(vsrc);
	if (decoder)
	{
		return decoder;
	}
	if (frameInfo.fourcc == *(UInt32*)"MP2G")
	{
		Media::Decoder::MP2GDecoder *mp2g;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(mp2g, Media::Decoder::MP2GDecoder(vsrc, false));
		NEW_CLASS(decoder, Media::Decoder::VFWDecoder(mp2g));
		if (decoder->GetVideoInfo(&decFrameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize))
		{
			NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
			decChain->AddDecoder(mp2g);
			return decChain;
		}
		DEL_CLASS(decoder);
		DEL_CLASS(mp2g);
	}
	else if (frameInfo.fourcc == *(UInt32*)"m2v1")
	{
		Media::Decoder::M2VDecoder *m2vd;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(m2vd, Media::Decoder::M2VDecoder(vsrc, false));
		NEW_CLASS(decoder, Media::Decoder::VFWDecoder(m2vd));
		if (decoder->GetVideoInfo(&decFrameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize))
		{
			NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
			decChain->AddDecoder(m2vd);
			return decChain;
		}
		DEL_CLASS(decoder);
		DEL_CLASS(m2vd);
	}
	else if (frameInfo.fourcc == *(UInt32*)"ravc")
	{
		Media::Decoder::RAVCDecoder *ravc;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(ravc, Media::Decoder::RAVCDecoder(vsrc, false, false));
		NEW_CLASS(decoder, Media::Decoder::VFWDecoder(ravc));
		if (decoder->GetVideoInfo(&decFrameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize))
		{
			NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
			decChain->AddDecoder(ravc);
			return decChain;
		}
		DEL_CLASS(decoder);
		DEL_CLASS(ravc);
	}
	else if (frameInfo.fourcc == *(UInt32*)"rhvc")
	{
		Media::Decoder::RHVCDecoder *rhvc;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(rhvc, Media::Decoder::RHVCDecoder(vsrc, false));
		NEW_CLASS(decoder, Media::Decoder::VFWDecoder(rhvc));
		if (decoder->GetVideoInfo(&decFrameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize))
		{
			NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
			decChain->AddDecoder(rhvc);
			return decChain;
		}
		DEL_CLASS(decoder);
		DEL_CLASS(rhvc);
	}

	NEW_CLASS(decoder, Media::Decoder::VFWDecoder(vsrc));
	if (decoder->GetVideoInfo(&decFrameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize))
	{
		return decoder;
	}
	DEL_CLASS(decoder);
	return 0;
}
