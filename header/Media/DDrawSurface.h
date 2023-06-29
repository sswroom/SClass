#ifndef _SM_MEDIA_DDRAWSURFACE
#define _SM_MEDIA_DDRAWSURFACE
#include "Handles.h"
#include "Media/Image.h"
#include "Media/MonitorSurface.h"

namespace Media
{
	class DDrawManager;

	class DDrawSurface : public Media::MonitorSurface
	{
	private:
		struct ClassData;

		ClassData *clsData;

	public:
		DDrawSurface(DDrawManager *mgr, void *lpDD, void *surface, MonitorHandle *hMon, Bool needRelease, Media::RotateType rotateType);
		virtual ~DDrawSurface();

		virtual Media::Image *Clone() const;
		virtual Media::Image::ImageType GetImageType() const;
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(Media::MonitorSurface *surface, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> drawSize, Bool clearScn, Bool waitForVBlank);
		virtual UInt8 *LockSurface(OSInt *lineAdd);
		virtual void UnlockSurface();
		virtual void SetSurfaceBugMode(Bool surfaceBugMode);

		void SetClipWindow(ControlHandle *clipWindow);
		void SetBuffSurface(Media::DDrawSurface *buffSurface);
	};
}
#endif
