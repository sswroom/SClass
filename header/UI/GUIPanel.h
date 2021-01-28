#ifndef _SM_UI_GUIPANEL
#define _SM_UI_GUIPANEL
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIPanel : public GUIClientControl
	{
	private:
		Int32 minW;
		Int32 minH;
		Bool scrollH;
		Bool scrollV;
		Int32 currScrX;
		Int32 currScrY;

		static Int32 useCnt;
		static OSInt __stdcall PnlWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		static void Init(void *hInst);
		static void Deinit(void *hInst);
		void UpdateScrollBars();
	public:
		GUIPanel(GUICore *ui, void *parentHWnd);
		GUIPanel(GUICore *ui, UI::GUIClientControl *parent);
		virtual ~GUIPanel();

		virtual Bool IsChildVisible();

		virtual void GetClientOfst(Double *x, Double *y);
		virtual void GetClientSize(Double *w, Double *h);
		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(Int32 code, void *lParam);

		void SetMinSize(Int32 minW, Int32 minH);
	};
};
#endif
