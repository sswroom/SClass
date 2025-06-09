#ifndef _SM_MEDIA_MONITORSURFACEMGR
#define _SM_MEDIA_MONITORSURFACEMGR
#include "Handles.h"
#include "Media/MonitorSurface.h"

namespace Media
{
	class MonitorSurfaceMgr
	{
	public:
		virtual ~MonitorSurfaceMgr() {};

		virtual Bool SetFSMode(Optional<MonitorHandle> hMon, Optional<ControlHandle> hWnd, Bool fs) = 0;
		virtual void WaitForVBlank(Optional<MonitorHandle> hMon) = 0;
		virtual UInt32 GetRefreshRate(Optional<MonitorHandle> hMon) = 0;
		virtual Optional<MonitorHandle> GetMonitorHandle(UOSInt monIndex) = 0;
		virtual UOSInt GetMonitorCount() = 0;

		virtual Optional<MonitorSurface> CreateSurface(Math::Size2D<UOSInt> size, UOSInt bitDepth) = 0;
		virtual Optional<MonitorSurface> CreatePrimarySurface(Optional<MonitorHandle> hMon, Optional<ControlHandle> clipWindow, Media::RotateType rotateType) = 0;
		virtual Bool CreatePrimarySurfaceWithBuffer(Optional<MonitorHandle> hMon, OutParam<NN<MonitorSurface>> primarySurface, OutParam<NN<MonitorSurface>> bufferSurface, Media::RotateType rotateType) = 0;
	};
}
#endif
