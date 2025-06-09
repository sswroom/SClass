#ifndef _SM_UI_WIN_WINHSCROLLBAR
#define _SM_UI_WIN_WINHSCROLLBAR
#include "UI/GUIHScrollBar.h"

namespace UI
{
	namespace Win
	{
		class WinHScrollBar : public GUIHScrollBar
		{
		private:
			static Int32 useCnt;

			static OSInt __stdcall FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
			static void Init(Optional<InstanceHandle> hInst);
			static void Deinit(Optional<InstanceHandle> hInst);
		public:
			WinHScrollBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Int32 width);
			virtual ~WinHScrollBar();

			virtual void InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg);
			virtual void SetPos(UOSInt pos);
			virtual UOSInt GetPos();
			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual void SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
		};
	}
}
#endif
