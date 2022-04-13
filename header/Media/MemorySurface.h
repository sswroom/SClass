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
		MemorySurface(UOSInt width, UOSInt height, UOSInt bitPerPixel, const Media::ColorProfile *color, Double dpi);
		virtual ~MemorySurface();

		Bool IsError();
		
		virtual Media::Image *Clone();
		virtual Media::Image::ImageType GetImageType();
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown);

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(Media::MonitorSurface *surface, Bool waitForVBlank);
		virtual Bool DrawFromMem(UInt8 *buff, OSInt lineAdd, OSInt destX, OSInt destY, UOSInt buffW, UOSInt buffH, Bool clearScn, Bool waitForVBlank);
		virtual UInt8 *LockSurface(OSInt *lineAdd);
		virtual void UnlockSurface();
	};
}
#endif
