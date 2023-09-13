#ifndef _SM_MEDIA_DECODER_VIDEODECODERFINDER
#define _SM_MEDIA_DECODER_VIDEODECODERFINDER

#include "Media/IVideoSource.h"

namespace Media
{
	namespace Decoder
	{
		class VideoDecoderFinder
		{
		public:
			VideoDecoderFinder();
			~VideoDecoderFinder();

			Media::IVideoSource *DecodeVideo(NotNullPtr<Media::IVideoSource> video);
		};
	}
}
#endif
