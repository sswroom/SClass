#ifndef _SM_MEDIA_MONITORSURFACE
#define _SM_MEDIA_MONITORSURFACE
#include "Media/Image.h"

namespace Media
{
	class MonitorSurface : public Media::Image
	{
	public:
		MonitorSurface() : Media::Image(0, 0) {};
		virtual ~MonitorSurface() {};

		virtual void WaitForVBlank() = 0;
		virtual void *GetHandle() = 0;

		virtual Bool DrawFromBuff() = 0;
		Bool DrawFromSurface(Media::MonitorSurface* surface, Bool waitForVBlank) { return this->DrawFromSurface(surface, 0, 0, surface->info.dispWidth, surface->info.dispHeight, false, waitForVBlank); };
		virtual Bool DrawFromSurface(Media::MonitorSurface* surface, OSInt destX, OSInt destY, UOSInt buffW, UOSInt buffH, Bool clearScn, Bool waitForVBlank) = 0;
		virtual Bool DrawFromMem(UInt8 *buff, OSInt lineAdd, OSInt destX, OSInt destY, UOSInt buffW, UOSInt buffH, Bool clearScn, Bool waitForVBlank) = 0;
		virtual UInt8 *LockSurface(OSInt *lineAdd) = 0;
		virtual void UnlockSurface() = 0;
		virtual void SetSurfaceBugMode(Bool surfaceBugMode) = 0;
	};
}
#endif
