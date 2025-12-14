#ifndef _SM_UI_JAVAUI_JUITEXTBOX
#define _SM_UI_JAVAUI_JUITEXTBOX
#include "UI/GUITextBox.h"

namespace UI
{
	namespace JavaUI
	{
		class JUITextBox : public GUITextBox
		{
		private:
			Bool multiline;
		public:
			JUITextBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline);
			virtual ~JUITextBox();

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
