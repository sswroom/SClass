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
			WinVSplitter(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Int32 height, Bool isBottom);
			virtual ~WinVSplitter();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos);
		};
	}
}
#endif
