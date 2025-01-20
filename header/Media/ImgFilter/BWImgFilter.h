#ifndef _SM_MEDIA_IMAGEFILTER_BWIMGFILTER
#define _SM_MEDIA_IMAGEFILTER_BWIMGFILTER
#include "Media/ImageFilter.h"

extern "C"
{
	void BWImgFilter_ProcYUY2(UInt8 *imgData, UOSInt pxCnt);
	void BWImgFilter_ProcUYVY(UInt8 *imgData, UOSInt pxCnt);
	void BWImgFilter_ProcAYUV(UInt8 *imgData, UOSInt pxCnt);
	void BWImgFilter_ProcY416(UInt8 *imgData, UOSInt pxCnt);
}

namespace Media
{
	namespace ImgFilter
	{
		class BWImgFilter : public Media::ImageFilter
		{
		private:
			Bool isEnable;
		public:
			BWImgFilter(Bool enable);
			virtual ~BWImgFilter();

			virtual void ProcessImage(UnsafeArray<UInt8> imgData, UInt32 imgFormat, UOSInt nBits, Media::PixelFormat pf, UOSInt imgWidth, UOSInt imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst);

			Bool IsEnabled();
			void SetEnabled(Bool enable);
		};
	}
}
#endif
