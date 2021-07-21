#ifndef _SM_UI_GUIGROUPBOX
#define _SM_UI_GUIGROUPBOX
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIGroupBox : public GUIClientControl
	{
	private:
		void *oriWndProc;

		static OSInt __stdcall GBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
	public:
		GUIGroupBox(GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *name);
		virtual ~GUIGroupBox();

		virtual void GetClientOfst(Double *x, Double *y);
		virtual void GetClientSize(Double *w, Double *h);
		virtual Bool IsChildVisible();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);
	};
};
#endif
