#ifndef _SM_UI_GUICUSTOMDRAWVSCROLL
#define _SM_UI_GUICUSTOMDRAWVSCROLL
#include "Media/DrawEngine.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUICustomDrawVScroll : public UI::GUIControl
	{
	public:
		NotNullPtr<Media::DrawEngine> deng;
		void *clsData;
		typedef enum
		{
			KBTN_NONE = 0,
			KBTN_SHIFT = 4,
			KBTN_CONTROL = 8
		} KeyButton;
	private:
		static Int32 useCnt;
		Data::ArrayList<UI::UIEvent> selChgHdlrs;
		Data::ArrayList<void *> selChgObjs;
		Data::ArrayList<UI::UIEvent> dblClkHdlrs;
		Data::ArrayList<void *> dblClkObjs;

	private:
		static OSInt __stdcall CDVSWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
		void OnPaint();
		void Init(void *hInst);
		void Deinit(void *hInst);
	
	protected:
		void ClearBackground(NotNullPtr<Media::DrawImage> img);

	public:
		GUICustomDrawVScroll(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, NotNullPtr<Media::DrawEngine> deng);
		virtual ~GUICustomDrawVScroll();

		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);

		virtual void OnDraw(NotNullPtr<Media::DrawImage> img) = 0;
		virtual void OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys);
		virtual void OnKeyDown(UInt32 keyCode);

		void HandleSelChg(UI::UIEvent hdlr, void *userObj);
		void HandleDblClk(UI::UIEvent hdlr, void *userObj);
		void EventSelChg();
		void EventDblClk();
		void SetVScrollBar(UOSInt min, UOSInt max, UOSInt pageSize);
		UOSInt GetVScrollPos();
		Bool MakeVisible(UOSInt firstIndex, UOSInt lastIndex);
	};
}
#endif
