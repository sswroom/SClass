#ifndef _SM_MEDIA_DDRAWMANAGER
#define _SM_MEDIA_DDRAWMANAGER
#include "Data/Int64Map.h"
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
		DDrawManager(UI::GUICore *ui, Media::ColorManagerSess *colorSess);
		DDrawManager(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr);
		virtual ~DDrawManager();

		Bool IsError();
		void *GetDD7(MonitorHandle *hMonitor);
		void ReleaseDD7(MonitorHandle *hMonitor);
		void RecheckMonitor();
		void Reinit();

		Double GetMonitorDPI(MonitorHandle *hMonitor);
		Bool Is10BitColor(MonitorHandle *hMonitor);
		Media::ColorProfile *GetMonProfile(MonitorHandle *hMonitor);

		virtual Bool SetFSMode(MonitorHandle *hMon, ControlHandle *hWnd, Bool fs);
		virtual void WaitForVBlank(MonitorHandle *hMon);
		virtual UInt32 GetRefreshRate(MonitorHandle *hMon);
		virtual MonitorSurface *CreateSurface(UOSInt width, UOSInt height, UOSInt bitDepth);
		virtual MonitorSurface *CreatePrimarySurface(MonitorHandle *hMon, ControlHandle *clipWindow);
		virtual Bool CreatePrimarySurfaceWithBuffer(MonitorHandle *hMon, MonitorSurface **primarySurface, MonitorSurface **bufferSurface);
	};
}
#endif
