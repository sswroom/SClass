#ifndef _SM_MEDIA_IMAGEFILTER_BWIMGFILTER
#define _SM_MEDIA_IMAGEFILTER_BWIMGFILTER
#include "Media/IImgFilter.h"

extern "C"
{
	void BWImgFilter_ProcYUY2(UInt8 *imgData, OSInt pxCnt);
	void BWImgFilter_ProcUYVY(UInt8 *imgData, OSInt pxCnt);
	void BWImgFilter_ProcAYUV(UInt8 *imgData, OSInt pxCnt);
	void BWImgFilter_ProcY416(UInt8 *imgData, OSInt pxCnt);
}

namespace Media
{
	namespace ImageFilter
	{
		class BWImgFilter : public Media::IImgFilter
		{
		private:
			Bool isEnable;
		public:
			BWImgFilter(Bool enable);
			virtual ~BWImgFilter();

			virtual void ProcessImage(UInt8 *imgData, Int32 imgFormat, OSInt nBits, Media::PixelFormat pf, OSInt imgWidth, OSInt imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst);

			Bool IsEnabled();
			void SetEnabled(Bool enable);
		};
	};
};
#endif
