#ifndef _SM_UI_GUICOREWIN
#define _SM_UI_GUICOREWIN
#include "UI/GUICore.h"

namespace UI
{
	class GUICoreWin : public UI::GUICore
	{
	private:
		void *hInst;
		Int32 frmCnt;
		Bool hasCommCtrl;
		Bool noDispOff;
		void *focusWnd;
		void *focusHAcc;
		Media::MonitorMgr *monMgr;

	public:
		GUICoreWin(void *hInst);
		virtual ~GUICoreWin();

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

		void SetFocusWnd(void *hWnd, void *hAcc);
		void *GetHInst();

		static OSInt MSGetWindowObj(ControlHandle *hWnd, OSInt index);
		static OSInt MSSetWindowObj(ControlHandle *hWnd, OSInt index, OSInt value);
		static OSInt MSSetClassObj(ControlHandle *hWnd, OSInt index, OSInt value);
	};
}
#endif
