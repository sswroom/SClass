#ifndef _SM_MEDIA_RASTERIMAGE
#define _SM_MEDIA_RASTERIMAGE
#include "Math/RectArea.hpp"
#include "Media/EXIFData.h"
#include "Media/FrameInfo.h"
#include "Media/Image.h"
#include "Text/CString.h"

namespace Media
{
	class StaticImage;

	class RasterImage : public Media::Image
	{
	public:
		enum class ImageClass
		{
			StaticImage,
			GUIImage,
			MonitorSurface
		};

	public:
		Media::FrameInfo info;
		Optional<Media::EXIFData> exif;
		Bool hasHotSpot;
		IntOS hotSpotX;
		IntOS hotSpotY;
		UnsafeArrayOpt<UInt8> pal;
		
	public:
		RasterImage(Math::Size2D<UIntOS> dispSize);
		RasterImage(Math::Size2D<UIntOS> dispSize, Math::Size2D<UIntOS> storeSize, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UIntOS maxSize, NN<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst);
		virtual ~RasterImage();

		virtual NN<Media::RasterImage> Clone() const = 0;
		virtual Media::RasterImage::ImageClass GetImageClass() const = 0;
		virtual void GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const = 0;
		void InitGrayPal();
		UIntOS GetDataBpl() const;
		Bool IsUpsideDown() const;
		void SetHotSpot(IntOS hotSpotX, IntOS hotSpotY);
		Bool HasHotSpot() const;
		IntOS GetHotSpotX() const;
		IntOS GetHotSpotY() const;

		virtual Bool IsRaster() const { return true; }
		virtual NN<Media::StaticImage> CreateStaticImage() const;
		virtual NN<Media::StaticImage> CreateSubImage(Math::RectArea<IntOS> area) const;
		Optional<Media::EXIFData> SetEXIFData(Optional<Media::EXIFData> exif);
		void ToString(NN<Text::StringBuilderUTF8> sb) const;

		Bool IsDispSize(Math::Size2D<UIntOS> dispSize);
	};
}
#endif
