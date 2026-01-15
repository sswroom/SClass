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
			virtual void SetFont(Text::CString fontName, Double fontHeightPt, Bool isBold);

			virtual IntOS OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
