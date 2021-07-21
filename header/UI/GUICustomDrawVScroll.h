#ifndef _SM_UI_GUICUSTOMDRAWVSCROLL
#define _SM_UI_GUICUSTOMDRAWVSCROLL
#include "Media/DrawEngine.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUICustomDrawVScroll : public UI::GUIControl
	{
	public:
		Media::DrawEngine *deng;
		void *clsData;
		typedef enum
		{
			KBTN_NONE = 0,
			KBTN_SHIFT = 4,
			KBTN_CONTROL = 8
		} KeyButton;
	private:
		static Int32 useCnt;
		Data::ArrayList<UI::UIEvent> *selChgHdlrs;
		Data::ArrayList<void *> *selChgObjs;
		Data::ArrayList<UI::UIEvent> *dblClkHdlrs;
		Data::ArrayList<void *> *dblClkObjs;

	private:
		static OSInt __stdcall CDVSWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
		void OnPaint();
		void Init(void *hInst);
		void Deinit(void *hInst);
	
	protected:
		void ClearBackground(Media::DrawImage *img);

	public:
		GUICustomDrawVScroll(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *deng);
		virtual ~GUICustomDrawVScroll();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnSizeChanged(Bool updateScn);

		virtual void OnDraw(Media::DrawImage *img) = 0;
		virtual void OnMouseDown(OSInt scrollY, Int32 xPos, Int32 yPos, UI::GUIClientControl::MouseButton btn, KeyButton keys);
		virtual void OnKeyDown(UInt32 keyCode);

		void HandleSelChg(UI::UIEvent hdlr, void *userObj);
		void HandleDblClk(UI::UIEvent hdlr, void *userObj);
		void EventSelChg();
		void EventDblClk();
		void SetVScrollBar(OSInt min, OSInt max, UOSInt pageSize);
		OSInt GetVScrollPos();
		Bool MakeVisible(OSInt index);
	};
}
#endif
