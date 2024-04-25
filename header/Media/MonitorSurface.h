#ifndef _SM_MEDIA_MONITORSURFACE
#define _SM_MEDIA_MONITORSURFACE
#include "Media/RasterImage.h"

namespace Media
{
	class MonitorSurface : public Media::RasterImage
	{
	public:
		MonitorSurface() : Media::RasterImage(Math::Size2D<UOSInt>(0, 0)) {};
		virtual ~MonitorSurface() {};

		virtual void WaitForVBlank() = 0;
		virtual void *GetHandle() = 0;

		virtual Bool DrawFromBuff() = 0;
		Bool DrawFromSurface(NN<Media::MonitorSurface> surface, Bool waitForVBlank) { return this->DrawFromSurface(surface, Math::Coord2D<OSInt>(0, 0), surface->info.dispSize, false, waitForVBlank); };
		virtual Bool DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn, Bool waitForVBlank) = 0;
		virtual UInt8 *LockSurface(OSInt *lineAdd) = 0;
		virtual void UnlockSurface() = 0;
		virtual void SetSurfaceBugMode(Bool surfaceBugMode) = 0;
	};
}
#endif
