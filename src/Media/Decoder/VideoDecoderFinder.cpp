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

Optional<Media::VideoSource> Media::Decoder::VideoDecoderFinder::DecodeVideo(NN<Media::VideoSource> vsrc)
{
	NN<Media::VideoSource> decoder;
	Media::FrameInfo frameInfo;
	Media::FrameInfo decFrameInfo;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UIntOS maxFrameSize;
	vsrc->GetVideoInfo(frameInfo, frameRateNorm, frameRateDenorm, maxFrameSize);
	if (frameInfo.fourcc == 0 || frameInfo.fourcc == 0xFFFFFFFF)
		return nullptr;

	Data::ArrayListUInt32 suppCSList;
	Media::CS::CSConverter::GetSupportedCS(suppCSList);
	if (suppCSList.IndexOf(frameInfo.fourcc) != INVALID_INDEX)
	{
		return nullptr;
	}
	if (Core::DecodeVideo(vsrc).SetTo(decoder))
	{
		return decoder;
	}
	if (frameInfo.fourcc == *(UInt32*)"MP2G")
	{
		NN<Media::Decoder::MP2GDecoder> mp2g;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASSNN(mp2g, Media::Decoder::MP2GDecoder(vsrc, false));
		NEW_CLASSNN(decoder, Media::Decoder::VFWDecoder(mp2g));
		if (decoder->GetVideoInfo(decFrameInfo, frameRateNorm, frameRateDenorm, maxFrameSize))
		{
			NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
			decChain->AddDecoder(mp2g);
			return decChain;
		}
		decoder.Delete();
		mp2g.Delete();
	}
	else if (frameInfo.fourcc == *(UInt32*)"m2v1")
	{
		NN<Media::Decoder::M2VDecoder> m2vd;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASSNN(m2vd, Media::Decoder::M2VDecoder(vsrc, false));
		NEW_CLASSNN(decoder, Media::Decoder::VFWDecoder(m2vd));
		if (decoder->GetVideoInfo(decFrameInfo, frameRateNorm, frameRateDenorm, maxFrameSize))
		{
			NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
			decChain->AddDecoder(m2vd);
			return decChain;
		}
		decoder.Delete();
		m2vd.Delete();
	}
	else if (frameInfo.fourcc == *(UInt32*)"ravc")
	{
		NN<Media::Decoder::RAVCDecoder> ravc;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASSNN(ravc, Media::Decoder::RAVCDecoder(vsrc, false, false));
		NEW_CLASSNN(decoder, Media::Decoder::VFWDecoder(ravc));
		if (decoder->GetVideoInfo(decFrameInfo, frameRateNorm, frameRateDenorm, maxFrameSize))
		{
			NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
			decChain->AddDecoder(ravc);
			return decChain;
		}
		decoder.Delete();
		ravc.Delete();
	}
	else if (frameInfo.fourcc == *(UInt32*)"rhvc")
	{
		NN<Media::Decoder::RHVCDecoder> rhvc;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASSNN(rhvc, Media::Decoder::RHVCDecoder(vsrc, false));
		NEW_CLASSNN(decoder, Media::Decoder::VFWDecoder(rhvc));
		if (decoder->GetVideoInfo(decFrameInfo, frameRateNorm, frameRateDenorm, maxFrameSize))
		{
			NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
			decChain->AddDecoder(rhvc);
			return decChain;
		}
		decoder.Delete();
		rhvc.Delete();
	}

	NEW_CLASSNN(decoder, Media::Decoder::VFWDecoder(vsrc));
	if (decoder->GetVideoInfo(decFrameInfo, frameRateNorm, frameRateDenorm, maxFrameSize))
	{
		return decoder;
	}
	decoder.Delete();
	return nullptr;
}
