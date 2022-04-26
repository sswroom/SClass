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
		virtual Double GetMagnifyRatio(MonitorHandle *hMonitor);
		virtual void UseDevice(Bool useSystem, Bool useDisplay);
		virtual void SetNoDisplayOff(Bool noDispOff);
		virtual void DisplayOff();
		virtual void Suspend();
		virtual void GetDesktopSize(UOSInt *w, UOSInt *h);
		virtual Math::Coord2D<OSInt> GetCursorPos();
		virtual void SetDisplayRotate(MonitorHandle *hMonitor, DisplayRotation rot);
		virtual void GetMonitorDPIs(MonitorHandle *hMonitor, Double *hdpi, Double *ddpi);
		virtual void SetMonitorMgr(Media::MonitorMgr *monMgr);
		virtual Bool IsForwarded();
	};
}
#endif
