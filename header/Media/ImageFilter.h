#ifndef _SM_MEDIA_IMAGEFILTER
#define _SM_MEDIA_IMAGEFILTER
#include "Media/FrameInfo.h"

namespace Media
{
	class ImageFilter
	{
	public:
		virtual ~ImageFilter(){}
		virtual void ProcessImage(UnsafeArray<UInt8> imgData, UInt32 imgFormat, UOSInt nBits, Media::PixelFormat pf, UOSInt imgWidth, UOSInt imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst) = 0;
	};
}
#endif
