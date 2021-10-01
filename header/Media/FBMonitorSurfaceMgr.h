#ifndef _SM_MEDIA_FBMONITORSURFACEMGR
#define _SM_MEDIA_FBMONITORSURFACEMGR
#include "Media/ColorManager.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"

namespace Media
{
	class FBMonitorSurfaceMgr : public MonitorSurfaceMgr
	{
	private:
		Media::MonitorMgr *monMgr;
		Media::ColorManager *colorMgr;
		Media::ColorManagerSess *colorSess;

	public:
		FBMonitorSurfaceMgr(Media::MonitorMgr *monMgr, Media::ColorManagerSess *colorSess);
		FBMonitorSurfaceMgr(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr);
		virtual ~FBMonitorSurfaceMgr();

		Double GetMonitorDPI(MonitorHandle *hMonitor);
		Media::ColorProfile *GetMonitorColor(MonitorHandle *hMonitor);
		Bool Is10BitColor(MonitorHandle *hMonitor);

		virtual Bool SetFSMode(MonitorHandle *hMon, ControlHandle *hWnd, Bool fs);
		virtual void WaitForVBlank(MonitorHandle *hMon);
		virtual UInt32 GetRefreshRate(MonitorHandle *hMon);
		virtual MonitorHandle *GetMonitorHandle(UOSInt monIndex);
		virtual UOSInt GetMonitorCount();

		virtual MonitorSurface *CreateSurface(UOSInt width, UOSInt height, UOSInt bitDepth);
		virtual MonitorSurface *CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow, Media::RotateType rotateType);
		virtual Bool CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface, Media::RotateType rotateType);
	};
}
#endif
