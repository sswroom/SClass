#ifndef _SM_UI_WIN_WINTEXTBOX
#define _SM_UI_WIN_WINTEXTBOX
#include "UI/GUITextBox.h"

namespace UI
{
	namespace Win
	{
		class WinTextBox : public GUITextBox
		{
		public:
			struct ClassData;
		private:
			ClassData *clsData;

		public:
			WinTextBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText);
			WinTextBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool isMultiline);
			virtual ~WinTextBox();

			virtual void SetReadOnly(Bool isReadOnly);
			virtual void SetPasswordChar(UTF32Char c);

			virtual void SetText(Text::CStringNN text);
			virtual UnsafeArrayOpt<UTF8Char> GetText(UnsafeArray<UTF8Char> buff);
			virtual Bool GetText(NN<Text::StringBuilderUTF8> sb);
			virtual OSInt OnNotify(UInt32 code, void *lParam);

			virtual void SetWordWrap(Bool wordWrap);
			virtual void SelectAll();
		};
	}
}
#endif
