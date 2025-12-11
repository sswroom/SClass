#ifndef _SM_UI_JAVAUI_JAVATEXTBOX
#define _SM_UI_JAVAUI_JAVATEXTBOX
#include "UI/GUITextBox.h"

namespace UI
{
	namespace JavaUI
	{
		class JavaTextBox : public GUITextBox
		{
		public:
			JavaTextBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline);
			virtual ~JavaTextBox();

			virtual void SetReadOnly(Bool isReadOnly);
			virtual void SetPasswordChar(UTF32Char c);

			virtual void SetText(Text::CStringNN text);
			virtual UnsafeArrayOpt<UTF8Char> GetText(UnsafeArray<UTF8Char> buff);
			virtual Bool GetText(NN<Text::StringBuilderUTF8> sb);
			virtual OSInt OnNotify(UInt32 code, void *lParam);

			void SetWordWrap(Bool wordWrap);
			void SelectAll();
		};
	}
}
#endif
