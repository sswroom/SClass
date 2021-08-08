#ifndef _SM_UI_GUICOREGTK
#define _SM_UI_GUICOREGTK
#include "UI/GUICore.h"

namespace UI
{
	class GUICoreGTK : public GUICore
	{
	private:
		Media::MonitorMgr *monMgr;
	public:
		GUICoreGTK();
		virtual ~GUICoreGTK();

		virtual void Run();
		virtual void ProcessMessages();
		virtual void WaitForMessages();
		virtual void Exit();
		virtual Media::DrawEngine *CreateDrawEngine();
		virtual Double GetMagnifyRatio(MonitorHandle *hMonitor);
		virtual void UseDevice(Bool useSystem, Bool useDisplay);
		virtual void SetNoDisplayOff(Bool noDispOff);
		virtual void DisplayOff();
		virtual void Suspend();
		virtual void GetDesktopSize(UOSInt *w, UOSInt *h);
		virtual void GetCursorPos(OSInt *x, OSInt *y);
		virtual void SetDisplayRotate(MonitorHandle *hMonitor, DisplayRotation rot);
		virtual void GetMonitorDPIs(MonitorHandle *hMonitor, Double *hdpi, Double *ddpi);
		virtual void SetMonitorMgr(Media::MonitorMgr *monMgr);
	};
}
#endif
