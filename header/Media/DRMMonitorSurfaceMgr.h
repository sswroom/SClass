#ifndef _SM_MEDIA_DRMMONITORSURFACEMGR
#define _SM_MEDIA_DRMMONITORSURFACEMGR
#include "Media/ColorManager.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"

namespace UI
{
	class GUICore;
}
namespace Media
{
	class DRMMonitorSurfaceMgr : public MonitorSurfaceMgr
	{
	private:
		class ClassData;

		NN<ClassData> clsData;
		Optional<Media::MonitorMgr> monMgr;
		Optional<Media::ColorManager> colorMgr;
		Optional<Media::ColorManagerSess> colorSess;
		Optional<UI::GUICore> ui;

		Bool OpenCard(UIntOS cardNum);
	public:
		DRMMonitorSurfaceMgr(UIntOS cardNum, NN<UI::GUICore> ui, NN<Media::ColorManagerSess> colorSess);
		DRMMonitorSurfaceMgr(UIntOS cardNum, Optional<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr);
		virtual ~DRMMonitorSurfaceMgr();

		Double GetMonitorDPI(Optional<MonitorHandle> hMonitor);
		Optional<const Media::ColorProfile> GetMonitorColor(Optional<MonitorHandle> hMonitor);
		Bool Is10BitColor(Optional<MonitorHandle> hMonitor);

		virtual Bool SetFSMode(Optional<MonitorHandle> hMon, Optional<ControlHandle> hWnd, Bool fs);
		virtual void WaitForVBlank(Optional<MonitorHandle> hMon);
		virtual UInt32 GetRefreshRate(Optional<MonitorHandle> hMon);
		virtual Optional<MonitorHandle> GetMonitorHandle(UIntOS monIndex);
		virtual UIntOS GetMonitorCount();

		virtual Optional<MonitorSurface> CreateSurface(Math::Size2D<UIntOS> size, UIntOS bitDepth);
		virtual Optional<MonitorSurface> CreatePrimarySurface(Optional<MonitorHandle> hMon, Optional<ControlHandle> clipWindow, Media::RotateType rotateType);
		virtual Bool CreatePrimarySurfaceWithBuffer(Optional<MonitorHandle> hMon, OutParam<NN<MonitorSurface>> primarySurface, OutParam<NN<MonitorSurface>> bufferSurface, Media::RotateType rotateType);
	};
}
#endif
