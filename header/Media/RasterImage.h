#ifndef _SM_MEDIA_RASTERIMAGE
#define _SM_MEDIA_RASTERIMAGE
#include "Math/RectArea.h"
#include "Media/FrameInfo.h"
#include "Media/EXIFData.h"
#include "Text/CString.h"

namespace Media
{
	class StaticImage;

	class RasterImage
	{
	public:
		enum class ImageType
		{
			Static,
			GUIImage,
			MonitorSurface
		};

	public:
		Media::FrameInfo info;
		Optional<Media::EXIFData> exif;
		Bool hasHotSpot;
		OSInt hotSpotX;
		OSInt hotSpotY;
		UnsafeArrayOpt<UInt8> pal;
		
	public:
		RasterImage(Math::Size2D<UOSInt> dispSize);
		RasterImage(Math::Size2D<UOSInt> dispSize, Math::Size2D<UOSInt> storeSize, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UOSInt maxSize, NN<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst);
		virtual ~RasterImage();

		virtual NN<Media::RasterImage> Clone() const = 0;
		virtual Media::RasterImage::ImageType GetImageType() const = 0;
		virtual void GetRasterData(UnsafeArray<UInt8> destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const = 0;
		void InitGrayPal();
		UOSInt GetDataBpl() const;
		Bool IsUpsideDown() const;
		void SetHotSpot(OSInt hotSpotX, OSInt hotSpotY);
		Bool HasHotSpot() const;
		OSInt GetHotSpotX() const;
		OSInt GetHotSpotY() const;

		NN<Media::StaticImage> CreateStaticImage() const;
		NN<Media::StaticImage> CreateSubImage(Math::RectArea<OSInt> area) const;
		Optional<Media::EXIFData> SetEXIFData(Optional<Media::EXIFData> exif);
		void ToString(NN<Text::StringBuilderUTF8> sb) const;

		Bool IsDispSize(Math::Size2D<UOSInt> dispSize);
	};
}
#endif
