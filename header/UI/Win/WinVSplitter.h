#ifndef _SM_UI_WIN_WINVSPLITTER
#define _SM_UI_WIN_WINVSPLITTER
#include "UI/GUIVSplitter.h"
#include <windows.h>

namespace UI
{
	namespace Win
	{
		class WinVSplitter : public GUIVSplitter
		{
		private:
			Bool isBottom;
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
			static void Init(InstanceHandle *hInst);
			static void Deinit(InstanceHandle *hInst);

			void DrawXorBar(HDC hdc, OSInt x, OSInt y);
			void CalDragRange();
		public:
			WinVSplitter(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Int32 height, Bool isBottom);
			virtual ~WinVSplitter();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
		};
	}
}
#endif
