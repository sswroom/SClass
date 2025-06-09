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

		Double GetMonitorDPI(Optional<MonitorHandle> hMonitor);
		Optional<const Media::ColorProfile> GetMonitorColor(Optional<MonitorHandle> hMonitor);
		Bool Is10BitColor(Optional<MonitorHandle> hMonitor);

		virtual Bool SetFSMode(Optional<MonitorHandle> hMon, Optional<ControlHandle> hWnd, Bool fs);
		virtual void WaitForVBlank(Optional<MonitorHandle> hMon);
		virtual UInt32 GetRefreshRate(Optional<MonitorHandle> hMon);
		virtual Optional<MonitorHandle> GetMonitorHandle(UOSInt monIndex);
		virtual UOSInt GetMonitorCount();

		virtual Optional<MonitorSurface> CreateSurface(Math::Size2D<UOSInt> size, UOSInt bitDepth);
		virtual Optional<MonitorSurface> CreatePrimarySurface(Optional<MonitorHandle> hMon, Optional<ControlHandle> clipWindow, Media::RotateType rotateType);
		virtual Bool CreatePrimarySurfaceWithBuffer(Optional<MonitorHandle> hMon, OutParam<NN<MonitorSurface>> primarySurface, OutParam<NN<MonitorSurface>> bufferSurface, Media::RotateType rotateType);
	};
}
#endif
