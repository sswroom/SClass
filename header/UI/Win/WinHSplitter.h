#ifndef _SM_UI_WIN_WINHSPLITTER
#define _SM_UI_WIN_WINHSPLITTER
#include "UI/GUIHSplitter.h"
#include <windows.h>

namespace UI
{
	namespace Win
	{
		class WinHSplitter : public UI::GUIHSplitter
		{
		private:
			Bool isRight;
			Bool dragMode;
			OSInt dragX;
			OSInt dragY;
			OSInt lastX;
			OSInt lastY;
			OSInt dragMax;
			OSInt dragMin;

		private:
			static Int32 useCnt;
			static OSInt __stdcall FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
			static void Init(void *hInst);
			static void Deinit(void *hInst);

			void DrawXorBar(HDC hdc, OSInt x, OSInt y);
			void CalDragRange();
		public:
			WinHSplitter(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Int32 width, Bool isRight);
			virtual ~WinHSplitter();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
		};
	}
}
#endif
