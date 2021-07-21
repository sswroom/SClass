#ifndef _SM_UI_GUITEXTBOX
#define _SM_UI_GUITEXTBOX
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITextBox : public GUIControl
	{
	private:
		Data::ArrayList<UI::UIEvent> *txtChgHdlrs;
		Data::ArrayList<void *> *txtChgObjs;
		void *clsData;

	public:
		GUITextBox(GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *initText);
		GUITextBox(GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *initText, Bool isMultiline);
		virtual ~GUITextBox();

		void EventTextChange();

		virtual void SetReadOnly(Bool isReadOnly);
		virtual void SetPasswordChar(UTF32Char c);

		virtual void SetText(const UTF8Char *text);
		virtual UTF8Char *GetText(UTF8Char *buff);
		virtual Bool GetText(Text::StringBuilderUTF *sb);
		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		virtual void HandleTextChanged(UI::UIEvent hdlr, void *userObj);
		void SelectAll();
	};
};

#endif
