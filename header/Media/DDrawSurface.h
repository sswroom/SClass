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

		virtual Media::Image *Clone();
		virtual Media::Image::ImageType GetImageType();
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown);

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(Media::MonitorSurface *surface, Bool waitForVBlank);
		virtual Bool DrawFromMem(UInt8 *buff, OSInt lineAdd, OSInt tlx, OSInt tly, UOSInt drawW, UOSInt drawH, Bool clearScn, Bool waitForVBlank);
		virtual UInt8 *LockSurface(OSInt *lineAdd);
		virtual void UnlockSurface();

		void SetClipWindow(ControlHandle *clipWindow);
		void SetBuffSurface(Media::DDrawSurface *buffSurface);
	};
}
#endif
