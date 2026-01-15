#ifndef _SM_MEDIA_BITMAPUTIL
#define _SM_MEDIA_BITMAPUTIL
#include "Media/StaticImage.h"

namespace Media
{
	class BitmapUtil
	{
	public:
		static Optional<Media::StaticImage> ParseDIBBuffer(const UInt8 *dataBuff, UIntOS dataSize);
	};
}
#endif
