#ifndef _SM_MEDIA_STATICIMAGE
#define _SM_MEDIA_STATICIMAGE
#include "Media/DrawEngine.h"
#include "Media/FrameInfo.h"
#include "Media/Image.h"
#include "Media/ImageAlphaBlend.h"
#include "Parser/ParserList.h"

namespace Media
{
	class IImgResizer;

	class StaticImage : public Image
	{
	public:
		typedef enum
		{
			RT_CW90,
			RT_CW180,
			RT_CW270
		} RotateType;
	public:
		UInt8 *data;
	
		StaticImage(UOSInt dispWidth, UOSInt dispHeight, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UOSInt maxSize, const Media::ColorProfile *color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst);
		StaticImage(Media::FrameInfo *imgInfo);
		virtual ~StaticImage();

		virtual Media::Image *Clone();
		virtual Media::Image::ImageType GetImageType();
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown);

		Bool To32bpp();
		Bool To64bpp();
		Bool ToW8();
		Bool ToPal8();
		Bool FillColor(UInt32 color);
		Bool MultiplyAlpha(Double alpha);
		Bool MultiplyColor(UInt32 color);
		Bool Resize(Media::IImgResizer *resizer, UOSInt newWidth, UOSInt newHeight);
		Bool RotateImage(RotateType rtype);
		Double CalcPSNR(Media::StaticImage *simg);
		Double CalcAvgContrast(UOSInt *bgPxCnt);
		Double CalcColorRate();
	};
}
#endif
