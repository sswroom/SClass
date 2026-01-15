#ifndef _SM_MEDIA_DDRAWMANAGER
#define _SM_MEDIA_DDRAWMANAGER
#include "Media/ColorManager.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"
#include "UI/GUICore.h"

namespace Media
{
	class DDrawManager : public Media::MonitorSurfaceMgr
	{
	private:
		struct ClassData;
		ClassData *clsData;

		static Int32 __stdcall DDEnumMonCall(void *guid, Char *driverDesc, Char *driverName, void *context, void *hMonitor);
		void ReleaseAll();
	public:
		DDrawManager(Optional<Media::MonitorMgr> monMgr, NN<Media::ColorManagerSess> colorSess);
		DDrawManager(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr);
		virtual ~DDrawManager();

		Bool IsError();
		void *GetDD7(Optional<MonitorHandle> hMonitor);
		void ReleaseDD7(Optional<MonitorHandle> hMonitor);
		void RecheckMonitor();
		void Reinit();

		Double GetMonitorDPI(Optional<MonitorHandle> hMonitor);
		Bool Is10BitColor(Optional<MonitorHandle> hMonitor);
		Optional<const Media::ColorProfile> GetMonProfile(Optional<MonitorHandle> hMonitor);

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
