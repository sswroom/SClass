#ifndef _SM_MEDIA_JBIG2DECODER
#define _SM_MEDIA_JBIG2DECODER
#include "Data/ByteArray.h"
#include "Media/StaticImage.h"

namespace Media
{
	class JBIG2Decoder
	{
	public:
		JBIG2Decoder();
		~JBIG2Decoder();

		Optional<Media::StaticImage> DecodeImage(Data::ByteArrayR dataBuff, UIntOS width = 0, UIntOS height = 0) const;
		Optional<Media::StaticImage> DecodeImage(Data::ByteArrayR dataBuff, UIntOS width, UIntOS height, Data::ByteArrayR globalData) const;
	};
}
#endif
