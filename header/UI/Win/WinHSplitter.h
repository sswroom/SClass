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
			IntOS dragX;
			IntOS dragY;
			IntOS lastX;
			IntOS lastY;
			IntOS dragMax;
			IntOS dragMin;

		private:
			static Int32 useCnt;
			static IntOS __stdcall FormWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam);
			static void Init(Optional<InstanceHandle> hInst);
			static void Deinit(Optional<InstanceHandle> hInst);

			void DrawXorBar(HDC hdc, IntOS x, IntOS y);
			void CalDragRange();
		public:
			WinHSplitter(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Int32 width, Bool isRight);
			virtual ~WinHSplitter();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos);
		};
	}
}
#endif
