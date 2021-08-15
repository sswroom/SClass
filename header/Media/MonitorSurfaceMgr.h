#ifndef _SM_MEDIA_MONITORSURFACEMGR
#define _SM_MEDIA_MONITORSURFACEMGR
#include "Handles.h"
#include "Media/MonitorSurface.h"
#include "UI/GUIControl.h"

namespace Media
{
	class MonitorSurfaceMgr
	{
	public:
		virtual ~MonitorSurfaceMgr() {};

		virtual Bool SetFSMode(MonitorHandle *hMon, ControlHandle *hWnd, Bool fs) = 0;
		virtual void WaitForVBlank(MonitorHandle *hMon) = 0;
		virtual UInt32 GetRefreshRate(MonitorHandle *hMon) = 0;
		virtual MonitorHandle *GetMonitorHandle(UOSInt monIndex) = 0;
		virtual UOSInt GetMonitorCount() = 0;

		virtual MonitorSurface *CreateSurface(UOSInt width, UOSInt height, UOSInt bitDepth) = 0;
		virtual MonitorSurface *CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow) = 0;
		virtual Bool CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface) = 0;
	};
}
#endif
