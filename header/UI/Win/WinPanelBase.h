#ifndef _SM_UI_WIN_WINPANELBASE
#define _SM_UI_WIN_WINPANELBASE
#include "UI/GUIPanelBase.h"
#include "UI/Win/WinCore.h"

namespace UI
{
	namespace Win
	{
		class WinPanelBase : public UI::GUIPanelBase
		{
		protected:
			NN<GUIPanel> master;
			NN<WinCore> ui;
			Int32 minW;
			Int32 minH;
			Bool scrollH;
			Bool scrollV;
			Int32 currScrX;
			Int32 currScrY;

			static Int32 useCnt;
			static OSInt __stdcall PnlWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
			static void Init(InstanceHandle *hInst);
			static void Deinit(InstanceHandle *hInst);
			void UpdateScrollBars();
		public:
			WinPanelBase(NN<GUIPanel> master, NN<GUICore> ui, ControlHandle *parentHWnd);
			WinPanelBase(NN<GUIPanel> master, NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~WinPanelBase();

			virtual Math::Coord2DDbl GetClientOfst();
			virtual Math::Size2DDbl GetClientSize();
			virtual void SetMinSize(Int32 minW, Int32 minH);
		};
	}
}
#endif
