#ifndef _SM_UI_DDRAWUI
#define _SM_UI_DDRAWUI

//#import "dxguid.lib"
//#import "ddraw.lib"
#include "UI/GUIUI.h"
#include "UI/DDrawWindow.h"
#include "Data/ArrayList.h"

namespace UI
{
	class DDrawUI : public UI::GUIUI
	{
	private:
		void *hInst;
		void *hAcc;
		void *hAccWnd;
		void *lpDD;
		Bool classReg;
		Data::ArrayList<UI::DDrawWindow*> *wnds;

	public:
		DDrawUI(void *hInst);
		virtual ~DDrawUI();

	public:
		void UseAccel(Int32 resId, UI::DDrawWindow *wnd);
		void ExitUI();
		void RunUI();

		void *GetDD();
		Bool GetScreenSize(Int32 *width, Int32 *height);

		UI::DDrawWindow *NewDDrawWindow(const WChar *title, UI::DDrawWindow::UIMode mode, UI::DDrawWindow::UpdateMode upMode, UI::DDrawWindow::UpdateHdlr hdlr, Int32 initWidth, Int32 initHeight, Bool fastUI);
		OSInt WindowProc(void *hWnd, UInt32 msg, UInt32 wParam, Int32 lParam);
		void WindowClosed(UI::DDrawWindow *wnd);
	};
};

#endif
