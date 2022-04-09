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

	public:
		GUITextBox(GUICore *ui, UI::GUIClientControl *parent, Text::CString initText);
		GUITextBox(GUICore *ui, UI::GUIClientControl *parent, Text::CString initText, Bool isMultiline);
		virtual ~GUITextBox();

		void EventTextChange();

		virtual void SetReadOnly(Bool isReadOnly);
		virtual void SetPasswordChar(UTF32Char c);

		virtual void SetText(Text::CString text);
		virtual UTF8Char *GetText(UTF8Char *buff);
		virtual Bool GetText(Text::StringBuilderUTF8 *sb);
		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		virtual void HandleTextChanged(UI::UIEvent hdlr, void *userObj);
		void SelectAll();
	};
};

#endif
