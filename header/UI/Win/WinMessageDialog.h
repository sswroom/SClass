#ifndef _SM_UI_WIN_WINMESSAGEDIALOG
#define _SM_UI_WIN_WINMESSAGEDIALOG
#include "UI/GUIControl.h"

namespace UI
{
	namespace Win
	{
		class WinMessageDialog
		{
		public:
			static void ShowOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
			static Bool ShowYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
		};
	}
}
#endif
