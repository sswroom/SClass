#ifndef _SM_MEDIA_GTKMONITORSURFACEMGR
#define _SM_MEDIA_GTKMONITORSURFACEMGR
#include "Media/ColorManager.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"

namespace Media
{
	class GTKMonitorSurfaceMgr : public MonitorSurfaceMgr
	{
	private:
		Media::MonitorMgr *monMgr;
		Media::ColorManager *colorMgr;
	public:
		GTKMonitorSurfaceMgr(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr);
		virtual ~GTKMonitorSurfaceMgr();

		virtual Bool SetFSMode(MonitorHandle *hMon, ControlHandle *hWnd, Bool fs);
		virtual void WaitForVBlank(MonitorHandle *hMon);
		virtual UInt32 GetRefreshRate(MonitorHandle *hMon);

		virtual MonitorSurface *CreateSurface(UOSInt width, UOSInt height, UOSInt bitDepth);
		virtual MonitorSurface *CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow);
		virtual Bool CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface);
	};
}
#endif
