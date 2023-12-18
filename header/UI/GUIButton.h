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
		Data::ArrayList<UIEvent> btnClkHandlers;
		Data::ArrayList<void *> btnClkHandlersObjs;
		Data::ArrayList<UpDownEvent> btnUpDownHandlers;
		Data::ArrayList<void *> btnUpDownHandlersObjs;
		Data::ArrayList<UIEvent> btnFocusLostHandlers;
		Data::ArrayList<void *> btnFocusLostHandlersObjs;
	public:
		GUIButton(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUIButton();

		virtual void SetText(Text::CStringNN text) = 0;
		virtual void SetFont(const UTF8Char *name, UOSInt nameLen, Double fontHeightPt, Bool isBold) = 0;
		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;

		virtual Text::CStringNN GetObjectClass() const;

		void EventFocus();
		void EventFocusLost();
		void EventButtonClick();
		void EventButtonDown();
		void EventButtonUp();

		void HandleFocusLost(UIEvent handler, void *userObj);
		void HandleButtonClick(UIEvent handler, void *userObj);
		void HandleButtonUpDown(UpDownEvent handler, void *userObj);
	};
}
#endif
