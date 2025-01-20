#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/ADXDecoder.h"
#include "Media/Decoder/MP2Decoder.h"
#include "Media/Decoder/PSSADecoder.h"
#include "Media/Decoder/XADecoder.h"
#include "Media/Decoder/ACMDecoder.h"
#include "Media/Decoder/G711muLawDecoder.h"

Media::Decoder::AudioDecoderFinder::AudioDecoderFinder()
{
}

Media::Decoder::AudioDecoderFinder::~AudioDecoderFinder()
{
}

Media::AudioSource *Media::Decoder::AudioDecoderFinder::DecodeAudio(NN<Media::AudioSource> asrc)
{
	AudioFormat afmt;
	Media::AudioSource *decoder;
	asrc->GetFormat(afmt);
	switch (afmt.formatId)
	{
	case 0x7: //G.711 mu-Law
		NEW_CLASS(decoder, Media::Decoder::G711muLawDecoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			DEL_CLASS(decoder);
			return 0;
		}
		return decoder;
	case 0x50: //MPEG-1 Audio Layer 2
		NEW_CLASS(decoder, Media::Decoder::MP2Decoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			DEL_CLASS(decoder);
			return 0;
		}
		return decoder;
	case 0x2080: //ADX
		NEW_CLASS(decoder, Media::Decoder::ADXDecoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			DEL_CLASS(decoder);
			return 0;
		}
		return decoder;
	case 0x2081: //PSSA
		NEW_CLASS(decoder, Media::Decoder::PSSADecoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			DEL_CLASS(decoder);
			return 0;
		}
		return decoder;
	case 0x2082: //XA
		NEW_CLASS(decoder, Media::Decoder::XADecoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			DEL_CLASS(decoder);
			return 0;
		}
		return decoder;
	default:
		decoder = Core::DecodeAudio(asrc);
		if (decoder)
			return decoder;

		NEW_CLASS(decoder, Media::Decoder::ACMDecoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			DEL_CLASS(decoder);
			return 0;
		}
		return decoder;
	}
}
