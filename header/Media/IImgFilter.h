#ifndef _SM_MEDIA_IIMGFILTER
#define _SM_MEDIA_IIMGFILTER
#include "Media/FrameInfo.h"

namespace Media
{
	class IImgFilter
	{
	public:
		virtual ~IImgFilter(){}
		virtual void ProcessImage(UInt8 *imgData, Int32 imgFormat, OSInt nBits, Media::PixelFormat pf, OSInt imgWidth, OSInt imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst) = 0;
	};
};
#endif
