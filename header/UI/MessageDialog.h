#ifndef _SM_UI_MESSAGEDIALOG
#define _SM_UI_MESSAGEDIALOG
#include "UI/GUIControl.h"

namespace UI
{
	class MessageDialog
	{
	public:
		static void ShowDialog(const UTF8Char *message, const UTF8Char *title, UI::GUIControl *frm);
		static Bool ShowYesNoDialog(const UTF8Char *message, const UTF8Char *title, UI::GUIControl *frm);
	};
};
#endif
