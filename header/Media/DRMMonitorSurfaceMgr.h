#ifndef _SM_MEDIA_DRMMONITORSURFACEMGR
#define _SM_MEDIA_DRMMONITORSURFACEMGR
#include "Media/ColorManager.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"

namespace Media
{
	class DRMMonitorSurfaceMgr : public MonitorSurfaceMgr
	{
	private:
		class ClassData;

		ClassData *clsData;
		Media::MonitorMgr *monMgr;
		Media::ColorManager *colorMgr;
		Media::ColorManagerSess *colorSess;
		UI::GUICore *ui;

		Bool OpenCard(UIntOS cardNum);
	public:
		DRMMonitorSurfaceMgr(UIntOS cardNum, NN<UI::GUICore> ui, Media::ColorManagerSess *colorSess);
		DRMMonitorSurfaceMgr(UIntOS cardNum, Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr);
		virtual ~DRMMonitorSurfaceMgr();

		Double GetMonitorDPI(MonitorHandle *hMonitor);
		Media::ColorProfile *GetMonitorColor(MonitorHandle *hMonitor);
		Bool Is10BitColor(MonitorHandle *hMonitor);

		virtual Bool SetFSMode(MonitorHandle *hMon, ControlHandle *hWnd, Bool fs);
		virtual void WaitForVBlank(MonitorHandle *hMon);
		virtual UInt32 GetRefreshRate(MonitorHandle *hMon);
		virtual MonitorHandle *GetMonitorHandle(UIntOS monIndex);
		virtual UIntOS GetMonitorCount();

		virtual Optional<MonitorSurface> CreateSurface(Math::Size2D<UIntOS> size, UIntOS bitDepth);
		virtual Optional<MonitorSurface> CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow);
		virtual Bool CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface);
	};
}
#endif
