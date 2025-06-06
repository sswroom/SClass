#ifndef _SM_MEDIA_DECODER_VIDEODECODERFINDER
#define _SM_MEDIA_DECODER_VIDEODECODERFINDER

#include "Media/VideoSource.h"

namespace Media
{
	namespace Decoder
	{
		class VideoDecoderFinder
		{
		public:
			VideoDecoderFinder();
			~VideoDecoderFinder();

			Optional<Media::VideoSource> DecodeVideo(NN<Media::VideoSource> video);
		};
	}
}
#endif
