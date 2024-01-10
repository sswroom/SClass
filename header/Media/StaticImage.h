#ifndef _SM_MEDIA_STATICIMAGE
#define _SM_MEDIA_STATICIMAGE
#include "Data/ByteArray.h"
#include "Math/RectArea.h"
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
		enum class RotateType
		{
			NoRotate,
			CW90,
			CW180,
			CW270
		};
	public:
		UInt8 *data;
	
		StaticImage(Math::Size2D<UOSInt> dispSize, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UOSInt maxSize, NotNullPtr<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst);
		StaticImage(NotNullPtr<const Media::FrameInfo> imgInfo);
		virtual ~StaticImage();

		virtual NotNullPtr<Media::Image> Clone() const;
		virtual Media::Image::ImageType GetImageType() const;
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		Bool To32bpp();
		Bool To64bpp();
		Bool ToW8();
		Bool ToPal8();
		Bool FillColor(UInt32 color);
		Bool MultiplyAlpha(Double alpha);
		Bool MultiplyColor(UInt32 color);
		Bool Resize(Media::IImgResizer *resizer, Math::Size2D<UOSInt> newSize);
		Bool RotateImage(RotateType rtype);
		Double CalcPSNR(Media::StaticImage *simg) const;
		Double CalcAvgContrast(UOSInt *bgPxCnt) const;
		Double CalcColorRate() const;
		UInt8 *CreateNearPixelMask(Math::Coord2D<UOSInt> pxCoord, Int32 maxRate);
		Math::RectArea<UOSInt> CalcNearPixelRange(Math::Coord2D<UOSInt> pxCoord, Int32 maxRate);
		Data::ByteArray GetDataArray() const;

	private:
		void CalcNearPixelMaskH32(UInt8 *pixelMask, UOSInt x, UOSInt y, UInt8 *c, Int32 maxRate);
		void CalcNearPixelMaskV32(UInt8 *pixelMask, UOSInt x, UOSInt y, UInt8 *c, Int32 maxRate);
	};
}
#endif
