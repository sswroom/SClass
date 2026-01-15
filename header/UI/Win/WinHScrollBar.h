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

			static IntOS __stdcall FormWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam);
			static void Init(Optional<InstanceHandle> hInst);
			static void Deinit(Optional<InstanceHandle> hInst);
		public:
			WinHScrollBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Int32 width);
			virtual ~WinHScrollBar();

			virtual void InitScrollBar(UIntOS minVal, UIntOS maxVal, UIntOS currVal, UIntOS largeChg);
			virtual void SetPos(UIntOS pos);
			virtual UIntOS GetPos();
			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual void SetAreaP(IntOS left, IntOS top, IntOS right, IntOS bottom, Bool updateScn);
			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
		};
	}
}
#endif
