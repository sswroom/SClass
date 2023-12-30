#ifndef _SM_UI_WIN_WINHSCROLLBAR
#define _SM_UI_WIN_WINHSCROLLBAR
#include "UI/GUIHScrollBar.h"

namespace UI
{
	namespace Win
	{
		class WinHScrollBar : public GUIHScrollBar
		{
		public:
			typedef void (__stdcall *PosChgEvent)(void *userObj, UOSInt newPos);

		private:
			Data::ArrayList<PosChgEvent> posChgHdlrs;
			Data::ArrayList<void *> posChgObjs;

		private:
			static Int32 useCnt;

			static OSInt __stdcall FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
			static void Init(void *hInst);
			static void Deinit(void *hInst);
		public:
			WinHScrollBar(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Int32 width);
			virtual ~WinHScrollBar();

			void InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg);
			void SetPos(UOSInt pos);
			UOSInt GetPos();

			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual void SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn);
			virtual Text::CStringNN GetObjectClass() const;
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
		};
	}
}
#endif
