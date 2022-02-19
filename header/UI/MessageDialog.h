#ifndef _SM_UI_MESSAGEDIALOG
#define _SM_UI_MESSAGEDIALOG
#include "UI/GUIControl.h"

namespace UI
{
	class MessageDialog
	{
	public:
		static void ShowDialog(Text::CString message, Text::CString title, UI::GUIControl *frm);
		static Bool ShowYesNoDialog(Text::CString message, Text::CString title, UI::GUIControl *frm);
	};
};
#endif
