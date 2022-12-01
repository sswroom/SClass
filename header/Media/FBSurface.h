#ifndef _SM_MEDIA_FBSURFACE
#define _SM_MEDIA_FBSURFACE
#include "Handles.h"
#include "Media/MonitorSurface.h"

namespace Media
{
	class FBSurface : public Media::MonitorSurface
	{
	private:
		struct ClassData;

		ClassData *clsData;

		Bool UpdateToScreen(Bool waitForVBlank);
	public:
		FBSurface(MonitorHandle *hMon, const Media::ColorProfile *color, Double dpi, Media::RotateType rotateType);
		virtual ~FBSurface();

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

		void SetBuffSurface(Media::MonitorSurface *buffSurface);
	};
}
#endif
