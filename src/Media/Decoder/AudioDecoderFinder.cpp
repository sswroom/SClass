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

Optional<Media::AudioSource> Media::Decoder::AudioDecoderFinder::DecodeAudio(NN<Media::AudioSource> asrc)
{
	AudioFormat afmt;
	NN<Media::AudioSource> decoder;
	asrc->GetFormat(afmt);
	switch (afmt.formatId)
	{
	case 0x7: //G.711 mu-Law
		NEW_CLASSNN(decoder, Media::Decoder::G711muLawDecoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			decoder.Delete();
			return 0;
		}
		return decoder;
	case 0x50: //MPEG-1 Audio Layer 2
		NEW_CLASSNN(decoder, Media::Decoder::MP2Decoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			decoder.Delete();
			return 0;
		}
		return decoder;
	case 0x2080: //ADX
		NEW_CLASSNN(decoder, Media::Decoder::ADXDecoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			decoder.Delete();
			return 0;
		}
		return decoder;
	case 0x2081: //PSSA
		NEW_CLASSNN(decoder, Media::Decoder::PSSADecoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			decoder.Delete();
			return 0;
		}
		return decoder;
	case 0x2082: //XA
		NEW_CLASSNN(decoder, Media::Decoder::XADecoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			decoder.Delete();
			return 0;
		}
		return decoder;
	default:
		if (Core::DecodeAudio(asrc).SetTo(decoder))
			return decoder;

		NEW_CLASSNN(decoder, Media::Decoder::ACMDecoder(asrc));
		decoder->GetFormat(afmt);
		if (afmt.formatId == 0)
		{
			decoder.Delete();
			return 0;
		}
		return decoder;
	}
}
