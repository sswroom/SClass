#ifndef _SM_UI_GTK_GTKMESSAGEDIALOG
#define _SM_UI_GTK_GTKMESSAGEDIALOG
#include "UI/GUIControl.h"

namespace UI
{
	namespace GTK
	{
		class GTKMessageDialog
		{
		public:
			static void ShowOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
			static Bool ShowYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl);
		};
	}
}
#endif
