#ifndef _SM_UI_GUITEXTBOX
#define _SM_UI_GUITEXTBOX
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITextBox : public GUIControl
	{
	public:
		struct ClassData;
	private:
		ClassData *clsData;
		Data::ArrayList<UI::UIEvent> txtChgHdlrs;
		Data::ArrayList<void *> txtChgObjs;
		Data::ArrayList<UI::KeyEvent> keyDownHdlrs;
		Data::ArrayList<void *> keyDownObjs;

	public:
		GUITextBox(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, Text::CStringNN initText);
		GUITextBox(NotNullPtr<GUICore> ui, UI::GUIClientControl *parent, Text::CStringNN initText, Bool isMultiline);
		virtual ~GUITextBox();

		void EventTextChange();
		Bool EventKeyDown(UInt32 osKey);

		virtual void SetReadOnly(Bool isReadOnly);
		virtual void SetPasswordChar(UTF32Char c);

		virtual void SetText(Text::CStringNN text);
		virtual UTF8Char *GetText(UTF8Char *buff);
		virtual Bool GetText(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		virtual void HandleTextChanged(UI::UIEvent hdlr, void *userObj);
		virtual void HandleKeyDown(UI::KeyEvent hdlr, void *userObj);
		void SetWordWrap(Bool wordWrap);
		void SelectAll();
	};
};

#endif
