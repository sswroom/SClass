#ifndef _SM_UI_GUIBUTTON
#define _SM_UI_GUIBUTTON
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIButton : public GUIControl
	{
	public:
		typedef void (__stdcall *UpDownEvent)(void *userObj, Bool isDown);
	private:
		Data::ArrayList<UIEvent> *btnClkHandlers;
		Data::ArrayList<void *> *btnClkHandlersObjs;
		Data::ArrayList<UpDownEvent> *btnUpDownHandlers;
		Data::ArrayList<void *> *btnUpDownHandlersObjs;
		OSInt btnId;
		void *oriWndProc;
		static OSInt nextId;

		static OSInt __stdcall BTNWndProc(void *hWnd, UInt32 msg, UInt32 wParam, OSInt lParam);
	public:
		GUIButton(GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *txt);
		virtual ~GUIButton();

		virtual void SetText(const UTF8Char *text);
		virtual void SetFont(const UTF8Char *name, UOSInt nameLen, Double fontHeightPt, Bool isBold);

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);
		virtual void OnFocus();
		virtual void OnFocusLost();
		void EventButtonClick();
		void EventButtonDown();
		void EventButtonUp();
		OSInt GetBtnId();
		void SetDefaultBtnLook();

		void HandleButtonClick(UIEvent handler, void *userObj);
		void HandleButtonUpDown(UpDownEvent handler, void *userObj);
	};
}
#endif
