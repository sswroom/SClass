#ifndef _SM_UI_JAVAUI_JUIMESSAGEDIALOG
#define _SM_UI_JAVAUI_JUIMESSAGEDIALOG
#include "UI/GUIControl.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIMessageDialog
		{
		public:
			static void ShowOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
			static Bool ShowYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
		};
	}
}
#endif
