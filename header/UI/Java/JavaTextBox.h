#ifndef _SM_UI_JAVA_JAVATEXTBOX
#define _SM_UI_JAVA_JAVATEXTBOX
#include "UI/GUITextBox.h"

namespace UI
{
	namespace Java
	{
		class JavaTextBox : public GUITextBox
		{
		public:
			JavaTextBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline);
			virtual ~JavaTextBox();

			virtual void SetReadOnly(Bool isReadOnly);
			virtual void SetPasswordChar(UTF32Char c);

			virtual void SetText(Text::CStringNN text);
			virtual UTF8Char *GetText(UTF8Char *buff);
			virtual Bool GetText(NN<Text::StringBuilderUTF8> sb);
			virtual Text::CStringNN GetObjectClass() const;
			virtual OSInt OnNotify(UInt32 code, void *lParam);

			virtual void HandleTextChanged(UI::UIEvent hdlr, void *userObj);
			virtual void HandleKeyDown(UI::KeyEvent hdlr, void *userObj);
			void SetWordWrap(Bool wordWrap);
			void SelectAll();
		};
	}
}
#endif
