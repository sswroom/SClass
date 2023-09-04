#ifndef _SM_MEDIA_IMAGE
#define _SM_MEDIA_IMAGE
#include "Math/RectArea.h"
#include "Media/FrameInfo.h"
#include "Media/EXIFData.h"
#include "Text/CString.h"

namespace Media
{
	class StaticImage;

	class Image
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
		Media::EXIFData *exif;
		Bool hasHotSpot;
		OSInt hotSpotX;
		OSInt hotSpotY;
		UInt8 *pal;
		
	public:
		Image(Math::Size2D<UOSInt> dispSize);
		Image(Math::Size2D<UOSInt> dispSize, Math::Size2D<UOSInt> storeSize, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UOSInt maxSize, NotNullPtr<const Media::ColorProfile> color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst);
		virtual ~Image();

		virtual Media::Image *Clone() const = 0;
		virtual Media::Image::ImageType GetImageType() const = 0;
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const = 0;
		void InitGrayPal();
		UOSInt GetDataBpl() const;
		Bool IsUpsideDown() const;
		void SetHotSpot(OSInt hotSpotX, OSInt hotSpotY);
		Bool HasHotSpot() const;
		OSInt GetHotSpotX() const;
		OSInt GetHotSpotY() const;

		Media::StaticImage *CreateStaticImage() const;
		Media::StaticImage *CreateSubImage(Math::RectArea<OSInt> area) const;
		Media::EXIFData *SetEXIFData(Media::EXIFData *exif);
		void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;

		Bool IsDispSize(Math::Size2D<UOSInt> dispSize);
		static Text::CString AlphaTypeGetName(AlphaType atype);
	};
}
#endif
