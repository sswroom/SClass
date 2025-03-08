#ifndef _SM_MEDIA_JPEGDECODER
#define _SM_MEDIA_JPEGDECODER
#include "Data/ByteArray.h"
#include "Media/StaticImage.h"

namespace Media
{
	class JPEGDecoder
	{
	public:
		JPEGDecoder();
		~JPEGDecoder();

		Bool Decode(Data::ByteArrayR dataBuff, UnsafeArray<UInt8> imgPtr, UOSInt bpl, UOSInt maxWidth, UOSInt maxHeight, Media::PixelFormat pf) const;
		Optional<Media::StaticImage> DecodeImage(Data::ByteArrayR dataBuff) const;
	};
}
#endif
