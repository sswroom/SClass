#ifndef _SM_MEDIA_ZXINGREADER
#define _SM_MEDIA_ZXINGREADER
#include "Media/StaticImage.h"

namespace Media
{
	class ZXingReader
	{
	public:
		ZXingReader();
		~ZXingReader();

		UnsafeArrayOpt<UTF8Char> ReadY8(UnsafeArray<UTF8Char> buff, UnsafeArray<const UInt8> imgData, UIntOS width, UIntOS height, UIntOS lineSize) const;
		UnsafeArrayOpt<UTF8Char> ReadImg(UnsafeArray<UTF8Char> buff, NN<const Media::RasterImage> img) const;
	};
}
#endif