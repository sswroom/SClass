#ifndef _SM_MEDIA_ZXINGWRITER
#define _SM_MEDIA_ZXINGWRITER
#include "Media/StaticImage.h"

namespace Media
{
	class ZXingWriter
	{
	public:
		static Optional<Media::StaticImage> GenQRCode(Text::CStringNN content, Math::Size2D<UIntOS> outputSize);
	};
}
#endif