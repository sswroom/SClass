#ifndef _SM_UI_GUIHSPLITTER
#define _SM_UI_GUIHSPLITTER
#include "UI/GUIControl.h"
#include "Parser/ParserList.h"

namespace UI
{
	class GUIHSplitter : public GUIControl
	{
	private:
		Bool isRight;
		Bool dragMode;
		Int32 dragX;
		Int32 dragY;
		Int32 lastX;
		Int32 lastY;
		OSInt dragMax;
		OSInt dragMin;

	private:
		static Int32 useCnt;
		static OSInt __stdcall FormWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		static void Init(void *hInst);
		static void Deinit(void *hInst);

		void DrawXorBar(void *hdc, Int32 x, Int32 y);
		void CalDragRange();
	public:
		GUIHSplitter(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, Int32 width, Bool isRight);
		virtual ~GUIHSplitter();

		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
		void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
	};
}
#endif
