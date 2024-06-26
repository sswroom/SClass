#ifndef _SM_MEDIA_IIMGFILTER
#define _SM_MEDIA_IIMGFILTER
#include "Media/FrameInfo.h"

namespace Media
{
	class IImgFilter
	{
	public:
		virtual ~IImgFilter(){}
		virtual void ProcessImage(UnsafeArray<UInt8> imgData, UInt32 imgFormat, UOSInt nBits, Media::PixelFormat pf, UOSInt imgWidth, UOSInt imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst) = 0;
	};
}
#endif
