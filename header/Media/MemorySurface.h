#ifndef _SM_MEDIA_MEMORYSURFACE
#define _SM_MEDIA_MEMORYSURFACE
#include "Media/MonitorSurface.h"

namespace Media
{
	class MemorySurface : public Media::MonitorSurface
	{
	private:
		UInt8 *buffPtr;

	public:
		MemorySurface(UOSInt width, UOSInt height, UOSInt bitPerPixel, Media::ColorProfile *color, Double dpi);
		virtual ~MemorySurface();

		Bool IsError();
		
		virtual Media::Image *Clone();
		virtual Media::Image::ImageType GetImageType();
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown);

		virtual void WaitForVBlank();
		virtual void *GetHandle();
		virtual Bool DrawFromBuff();
	};
}
#endif
