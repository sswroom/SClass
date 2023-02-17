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

		UTF8Char *ReadY8(UTF8Char *buff, const UInt8 *imgData, UOSInt width, UOSInt height, UOSInt lineSize);
		UTF8Char *ReadImg(UTF8Char *buff, const Media::Image *img);
	};
}
#endif