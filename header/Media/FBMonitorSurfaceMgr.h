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
		Optional<Media::MonitorMgr> monMgr;
		Optional<Media::ColorManager> colorMgr;
		Optional<Media::ColorManagerSess> colorSess;

	public:
		FBMonitorSurfaceMgr(Optional<Media::MonitorMgr> monMgr, NN<Media::ColorManagerSess> colorSess);
		FBMonitorSurfaceMgr(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr);
		virtual ~FBMonitorSurfaceMgr();

		Double GetMonitorDPI(MonitorHandle *hMonitor);
		const Media::ColorProfile *GetMonitorColor(MonitorHandle *hMonitor);
		Bool Is10BitColor(MonitorHandle *hMonitor);

		virtual Bool SetFSMode(MonitorHandle *hMon, ControlHandle *hWnd, Bool fs);
		virtual void WaitForVBlank(MonitorHandle *hMon);
		virtual UInt32 GetRefreshRate(MonitorHandle *hMon);
		virtual MonitorHandle *GetMonitorHandle(UOSInt monIndex);
		virtual UOSInt GetMonitorCount();

		virtual MonitorSurface *CreateSurface(Math::Size2D<UOSInt> size, UOSInt bitDepth);
		virtual MonitorSurface *CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow, Media::RotateType rotateType);
		virtual Bool CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface, Media::RotateType rotateType);
	};
}
#endif
