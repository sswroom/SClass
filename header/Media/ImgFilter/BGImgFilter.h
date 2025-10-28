#ifndef _SM_MEDIA_IMAGEFILTER_BGIMGFILTER
#define _SM_MEDIA_IMAGEFILTER_BGIMGFILTER
#include "Media/ImageFilter.h"
#include "Media/ImgFilter/BGImgFilter_C.h"

namespace Media
{
	namespace ImgFilter
	{
		class BGImgFilter : public Media::ImageFilter
		{
		private:
			Bool isEnable;
			Bool isAbsolute;
			Bool toCreateBG;

			UInt32 bgFormat;
			UnsafeArrayOpt<UInt8> bgData;
			UOSInt bgWidth;
			UOSInt bgHeight;
		public:
			BGImgFilter();
			virtual ~BGImgFilter();

			virtual void ProcessImage(UnsafeArray<UInt8> imgData, UInt32 imgFormat, UOSInt nBits, Media::PixelFormat pf, UOSInt imgWidth, UOSInt imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst);

			Bool IsEnabled();
			Bool IsAbsolute();
			void SetEnabled(Bool enable);
			void SetAbsolute(Bool absolute);
			void ToCreateBGImg();

		};
	}
}
#endif
