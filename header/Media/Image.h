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
		typedef enum
		{
			IT_STATIC,
			IT_GUIIMAGE,
			IT_MONITORSURFACE
		} ImageType;

	public:
		Media::FrameInfo info;
		Media::EXIFData *exif;
		Bool hasHotSpot;
		OSInt hotSpotX;
		OSInt hotSpotY;
		UInt8 *pal;
		
	public:
		Image(UOSInt dispWidth, UOSInt dispHeight);
		Image(UOSInt dispWidth, UOSInt dispHeight, UOSInt storeWidth, UOSInt storeHeight, UInt32 fourcc, UInt32 bpp, Media::PixelFormat pf, UOSInt maxSize, const Media::ColorProfile *color, Media::ColorProfile::YUVType yuvType, Media::AlphaType atype, Media::YCOffset ycOfst);
		virtual ~Image();

		virtual Media::Image *Clone() const = 0;
		virtual Media::Image::ImageType GetImageType() const = 0;
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown) const = 0;
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
		void ToString(Text::StringBuilderUTF8 *sb) const;

		Bool IsDispSize(UOSInt dispWidth, UOSInt dispHeight);
		static Text::CString AlphaTypeGetName(AlphaType atype);
	};
}
#endif
