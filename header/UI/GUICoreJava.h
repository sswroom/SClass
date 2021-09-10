#ifndef _SM_UI_GUICOREJAVA
#define _SM_UI_GUICOREJAVA
#include "Sync/Event.h"
#include "UI/GUICore.h"

namespace UI
{
	class GUICoreJava : public GUICore
	{
	private:
		Bool toExit;
		Sync::Event *waitEvt;
		Media::MonitorMgr *monMgr;
	public:
		GUICoreJava();
		virtual ~GUICoreJava();

		virtual void Run();
		virtual void ProcessMessages();
		virtual void WaitForMessages();
		virtual void Exit();
		virtual Media::DrawEngine *CreateDrawEngine();
		virtual Double GetMagnifyRatio(void *hMonitor);
		virtual void UseDevice(Bool useSystem, Bool useDisplay);
		virtual void SetNoDisplayOff(Bool noDispOff);
		virtual void DisplayOff();
		virtual void Suspend();
		virtual void GetDesktopSize(UOSInt *w, UOSInt *h);
		virtual void GetCursorPos(OSInt *x, OSInt *y);
		virtual void SetDisplayRotate(void *hMonitor, DisplayRotation rot);
		virtual void GetMonitorDPIs(void *hMonitor, Double *hdpi, Double *ddpi);
		virtual void SetMonitorMgr(Media::MonitorMgr *monMgr);
		virtual Bool IsForwarded();
	};
}
#endif
