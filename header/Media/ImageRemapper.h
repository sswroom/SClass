#ifndef _SM_MEDIA_IMAGEREMAPPER
#define _SM_MEDIA_IMAGEREMAPPER
#include "Media/StaticImage.h"

namespace Media
{
	class ImageRemapper
	{
	protected:
		UnsafeArrayOpt<const UInt8> srcImgPtr;
		UIntOS srcBpl;
		UIntOS srcWidth;
		UIntOS srcHeight;
		Media::PixelFormat srcPF;
		UnsafeArrayOpt<const UInt8> srcPal;
		typedef UInt32 (CALLBACKFUNC GetPixel32Func)(UnsafeArray<const UInt8> srcImgPtr, Math::Coord2DDbl srcCoord, NN<ImageRemapper> self);
		typedef void (CALLBACKFUNC WritePixel32Func)(UnsafeArray<UInt8> destImgPtr, UInt32 px, Double alpha);
	protected:
		virtual GetPixel32Func GetPixel32() = 0;
		Bool Remap(UnsafeArray<UInt8> destImgPtr, UIntOS destBpl, UIntOS destWidth, UIntOS destHeight, Math::Quadrilateral destQuad, Double srcAlpha, WritePixel32Func func);
	public:
		ImageRemapper();
		virtual ~ImageRemapper(){};

		void SetSourceImage32(UnsafeArray<const UInt8> srcImgPtr, UIntOS srcBpl, UIntOS srcWidth, UIntOS srcHeight);
		void SetSourceImage(NN<Media::StaticImage> srcImg);
		Bool RemapDirect(UnsafeArray<UInt8> destImgPtr, UIntOS destBpl, UIntOS destWidth, UIntOS destHeight, Math::Quadrilateral destQuad);
		Bool RemapDirectAlpha(UnsafeArray<UInt8> destImgPtr, UIntOS destBpl, UIntOS destWidth, UIntOS destHeight, Math::Quadrilateral destQuad, Double srcAlpha);
		Bool RemapAlpha(UnsafeArray<UInt8> destImgPtr, UIntOS destBpl, UIntOS destWidth, UIntOS destHeight, Math::Quadrilateral destQuad, Double srcAlpha);
	};
}
#endif
