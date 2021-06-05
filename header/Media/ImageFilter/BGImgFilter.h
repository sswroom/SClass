#ifndef _SM_MEDIA_IMAGEFILTER_BGIMGFILTER
#define _SM_MEDIA_IMAGEFILTER_BGIMGFILTER
#include "Media/IImgFilter.h"

extern "C"
{
	void BGImgFilter_DiffA8_YUY2(UInt8 *destImg, UInt8 *bgImg, UOSInt pxCnt);
	void BGImgFilter_DiffYUV8(UInt8 *destImg, UInt8 *bgImg, UOSInt byteCnt);
}

namespace Media
{
	namespace ImageFilter
	{
		class BGImgFilter : public Media::IImgFilter
		{
		private:
			Bool isEnable;
			Bool isAbsolute;
			Bool toCreateBG;

			UInt32 bgFormat;
			UInt8 *bgData;
			UOSInt bgWidth;
			UOSInt bgHeight;
		public:
			BGImgFilter();
			virtual ~BGImgFilter();

			virtual void ProcessImage(UInt8 *imgData, UInt32 imgFormat, UOSInt nBits, Media::PixelFormat pf, UOSInt imgWidth, UOSInt imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst);

			Bool IsEnabled();
			Bool IsAbsolute();
			void SetEnabled(Bool enable);
			void SetAbsolute(Bool absolute);
			void ToCreateBGImg();

		};
	}
}
#endif
