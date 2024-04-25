#ifndef _SM_MEDIA_DECODER_AUDIODECODERFINDER
#define _SM_MEDIA_DECODER_AUDIODECODERFINDER

#include "Media/IAudioSource.h"

namespace Media
{
	namespace Decoder
	{
		class AudioDecoderFinder
		{
		public:
			AudioDecoderFinder();
			~AudioDecoderFinder();

			Media::IAudioSource *DecodeAudio(NN<Media::IAudioSource> audio);
		};
	}
}
#endif
