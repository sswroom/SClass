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

		Bool IsError() const;
		
		virtual Media::Image *Clone() const;
		virtual Media::Image::ImageType GetImageType() const;
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(Media::MonitorSurface *surface, OSInt destX, OSInt destY, UOSInt buffW, UOSInt buffH, Bool clearScn, Bool waitForVBlank);
		virtual UInt8 *LockSurface(OSInt *lineAdd);
		virtual void UnlockSurface();
		virtual void SetSurfaceBugMode(Bool surfaceBugMode);
	};
}
#endif
