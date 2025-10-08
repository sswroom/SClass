#ifndef _SM_MEDIA_IMAGEREMAPPER
#define _SM_MEDIA_IMAGEREMAPPER
#include "Media/StaticImage.h"

namespace Media
{
	class ImageRemapper
	{
	protected:
		UnsafeArrayOpt<const UInt8> srcImgPtr;
		UOSInt srcBpl;
		UOSInt srcWidth;
		UOSInt srcHeight;
		Media::PixelFormat srcPF;
		UnsafeArrayOpt<const UInt8> srcPal;
		typedef UInt32 (CALLBACKFUNC GetPixel32Func)(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self);

	protected:
		virtual GetPixel32Func GetPixel32() = 0;
	public:
		ImageRemapper();
		virtual ~ImageRemapper(){};

		void SetSourceImage32(UnsafeArray<const UInt8> srcImgPtr, UOSInt srcBpl, UOSInt srcWidth, UOSInt srcHeight);
		void SetSourceImage(NN<Media::StaticImage> srcImg);
		Bool Remap(UnsafeArray<UInt8> destImgPtr, UOSInt destBpl, UOSInt destWidth, UOSInt destHeight, Math::Quadrilateral destQuad);
	};
}
#endif
