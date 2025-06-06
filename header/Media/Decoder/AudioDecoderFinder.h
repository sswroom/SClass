#ifndef _SM_MEDIA_DECODER_AUDIODECODERFINDER
#define _SM_MEDIA_DECODER_AUDIODECODERFINDER

#include "Media/AudioSource.h"

namespace Media
{
	namespace Decoder
	{
		class AudioDecoderFinder
		{
		public:
			AudioDecoderFinder();
			~AudioDecoderFinder();

			Optional<Media::AudioSource> DecodeAudio(NN<Media::AudioSource> audio);
		};
	}
}
#endif
