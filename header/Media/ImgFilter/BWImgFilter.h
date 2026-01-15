#ifndef _SM_MEDIA_IMAGEFILTER_BWIMGFILTER
#define _SM_MEDIA_IMAGEFILTER_BWIMGFILTER
#include "Media/ImageFilter.h"
#include "Media/ImgFilter/BWImgFilter_C.h"

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

			virtual void ProcessImage(UnsafeArray<UInt8> imgData, UInt32 imgFormat, UIntOS nBits, Media::PixelFormat pf, UIntOS imgWidth, UIntOS imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst);

			Bool IsEnabled();
			void SetEnabled(Bool enable);
		};
	}
}
#endif
