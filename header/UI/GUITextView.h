#ifndef _SM_UI_GUITEXTVIEW
#define _SM_UI_GUITEXTVIEW
#include "Data/ArrayListUInt64.h"
#include "IO/FileStream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITextView : public GUIControl
	{
	private:
		static OSInt useCnt;
		Media::DrawEngine *deng;
		void *drawFont;

	protected:	
		UInt32 pageLineCnt;
		Int32 pageLineHeight;
		Media::DrawImage *drawBuff;

	private:
		static OSInt __stdcall TFVWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
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
		GUITextView(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *deng);
		virtual ~GUITextView();

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void UpdateFont();

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
		virtual void UpdateDrawBuff() = 0;
		virtual void UpdateCaretPos() = 0;
	};
}
#endif
