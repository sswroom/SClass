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
		void *GetDD7(MonitorHandle *hMonitor);
		void ReleaseDD7(MonitorHandle *hMonitor);
		void RecheckMonitor();
		void Reinit();

		Double GetMonitorDPI(MonitorHandle *hMonitor);
		Bool Is10BitColor(MonitorHandle *hMonitor);
		const Media::ColorProfile *GetMonProfile(MonitorHandle *hMonitor);

		virtual Bool SetFSMode(MonitorHandle *hMon, ControlHandle *hWnd, Bool fs);
		virtual void WaitForVBlank(MonitorHandle *hMon);
		virtual UInt32 GetRefreshRate(MonitorHandle *hMon);
		virtual MonitorHandle *GetMonitorHandle(UOSInt monIndex);
		virtual UOSInt GetMonitorCount();

		virtual Optional<MonitorSurface> CreateSurface(Math::Size2D<UOSInt> size, UOSInt bitDepth);
		virtual Optional<MonitorSurface> CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow, Media::RotateType rotateType);
		virtual Bool CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, OutParam<NN<MonitorSurface>> primarySurface, OutParam<NN<MonitorSurface>> bufferSurface, Media::RotateType rotateType);
	};
}
#endif
