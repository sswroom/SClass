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
		GUIPanel(NotNullPtr<GUICore> ui, void *parentHWnd);
		GUIPanel(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent);
		virtual ~GUIPanel();

		virtual Bool IsChildVisible();

		virtual Math::Coord2DDbl GetClientOfst();
		virtual Math::Size2DDbl GetClientSize();
		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		void SetMinSize(Int32 minW, Int32 minH);
	};
};
#endif
