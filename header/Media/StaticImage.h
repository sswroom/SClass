#ifndef _SM_MEDIA_STATICIMAGE
#define _SM_MEDIA_STATICIMAGE
#include "Data/ByteArray.h"
#include "Math/RectArea.hpp"
#include "Media/DrawEngine.h"
#include "Media/FrameInfo.h"
#include "Media/ImageAlphaBlend.h"
#include "Media/RasterImage.h"
#include "Parser/ParserList.h"

namespace Media
{
	class ImageResizer;

	class StaticImage : public RasterImage
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
		UnsafeArray<UInt8> data;
	
		StaticImage(Math::Size2D<UIntOS> dispSize, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UIntOS maxSize, NN<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst);
		StaticImage(NN<const Media::FrameInfo> imgInfo);
		virtual ~StaticImage();

		virtual NN<Media::RasterImage> Clone() const;
		virtual Media::RasterImage::ImageType GetImageType() const;
		virtual void GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		Bool ToB8G8R8A8();
		Bool ToB16G16R16A16();
		Bool ToW8();
		Bool ToPal8();
		Bool PalTo8bpp();
		Bool FillColor(UInt32 color);
		Bool MultiplyAlpha(Double alpha);
		Bool MultiplyColor(UInt32 color);
		Bool Resize(Media::ImageResizer *resizer, Math::Size2D<UIntOS> newSize);
		Bool RotateImage(RotateType rtype);
		Double CalcPSNR(NN<Media::StaticImage> simg) const;
		Double CalcAvgContrast(UIntOS *bgPxCnt) const;
		Double CalcColorRate() const;
		UInt8 *CreateNearPixelMask(Math::Coord2D<UIntOS> pxCoord, Int32 maxRate);
		Math::RectArea<UIntOS> CalcNearPixelRange(Math::Coord2D<UIntOS> pxCoord, Int32 maxRate);
		Data::ByteArray GetDataArray() const;

	private:
		void CalcNearPixelMaskH32(UnsafeArray<UInt8> pixelMask, UIntOS x, UIntOS y, UnsafeArray<UInt8> c, Int32 maxRate);
		void CalcNearPixelMaskV32(UnsafeArray<UInt8> pixelMask, UIntOS x, UIntOS y, UnsafeArray<UInt8> c, Int32 maxRate);
	};
}
#endif
