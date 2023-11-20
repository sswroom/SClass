#ifndef _SM_UI_GUITEXTVIEW
#define _SM_UI_GUITEXTVIEW
#include "Media/DrawEngine.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITextView : public GUIControl
	{
	private:
		struct ClassData;

		static OSInt useCnt;
		NotNullPtr<Media::DrawEngine> deng;
		ClassData *clsData;
		void *drawFont;
		Media::DrawImage *drawBuff;

	protected:	
		UInt32 pageLineCnt;
		Double pageLineHeight;
		UInt32 bgColor;
		UInt32 scrColor;
		UInt32 txtColor;
		UInt32 lineNumColor;
		UInt32 selColor;
		UInt32 selTextColor;

	private:
		static OSInt __stdcall TFVWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
		static void __stdcall OnResize(void *userObj);
		void Init(void *hInst);
		void Deinit(void *hInst);
		void OnPaint();
		void UpdateScrollBar();

	protected:
		Bool IsShiftPressed();
		void SetScrollHPos(UOSInt pos, Bool redraw);
		void SetScrollVPos(UOSInt pos, Bool redraw);
		void SetScrollHRange(UOSInt min, UOSInt max);
		void SetScrollVRange(UOSInt min, UOSInt max);

		UInt32 GetCharCntAtWidth(WChar *str, UOSInt strLen, UOSInt pxWidth);
		void GetDrawSize(WChar *str, UOSInt strLen, UOSInt *width, UOSInt *height);
		void SetCaretPos(OSInt scnX, OSInt scnY);
	public:
		GUITextView(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Media::DrawEngine> deng);
		virtual ~GUITextView();

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void UpdateFont();
		virtual OSInt GetScrollHPos();
		virtual OSInt GetScrollVPos();

		virtual void EventLineUp() = 0;
		virtual void EventLineDown() = 0;
		virtual void EventPageUp() = 0;
		virtual void EventPageDown() = 0;
		virtual void EventLeft() = 0;
		virtual void EventRight() = 0;
		virtual void EventHome() = 0;
		virtual void EventEnd() = 0;
		virtual void EventLineBegin() = 0;
		virtual void EventLineEnd() = 0;
		virtual void EventCopy() = 0;
		virtual void EventMouseDown(OSInt scnX, OSInt scnY, MouseButton btn) = 0;
		virtual void EventMouseUp(OSInt scnX, OSInt scnY, MouseButton btn) = 0;
		virtual void EventMouseMove(OSInt scnX, OSInt scnY) = 0;
		virtual void EventTimerTick() = 0;
		virtual void DrawImage(NotNullPtr<Media::DrawImage> dimg) = 0;
		virtual void UpdateCaretPos() = 0;

		void OnMouseDown(OSInt scnX, OSInt scnY, MouseButton btn);
		void OnMouseUp(OSInt scnX, OSInt scnY, MouseButton btn);
		void OnMouseMove(OSInt scnX, OSInt scnY);
		void OnMouseWheel(Bool isDown);
		void OnDraw(void *cr);
		void SetShiftState(Bool isDown);
	};
}
#endif
