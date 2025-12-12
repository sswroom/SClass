#ifndef _SM_UI_JAVAUI_JUIBUTTON
#define _SM_UI_JAVAUI_JUIBUTTON
#include "UI/GUIButton.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIButton : public UI::GUIButton
		{
		public:
			JUIButton(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN txt);
			virtual ~JUIButton();

			virtual void SetText(Text::CStringNN text);
			virtual void SetFont(UnsafeArrayOpt<const UTF8Char> name, UOSInt nameLen, Double fontHeightPt, Bool isBold);

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
