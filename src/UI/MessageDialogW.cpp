#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/MessageDialog.h"
#include <windows.h>

void UI::MessageDialog::ShowDialog(Text::CStringNN message, Text::CStringNN title, UI::GUIControl *ctrl)
{
	HWND hwnd = 0;
	if (ctrl)
		hwnd = (HWND)ctrl->GetHandle();
	const WChar *wmessage = Text::StrToWCharNew(message.v);
	const WChar *wtitle = Text::StrToWCharNew(title.v);
	MessageBoxW(hwnd, wmessage, wtitle, MB_OK);
	Text::StrDelNew(wmessage);
	Text::StrDelNew(wtitle);
}

Bool UI::MessageDialog::ShowYesNoDialog(Text::CStringNN message, Text::CStringNN title, UI::GUIControl *ctrl)
{
	HWND hwnd = 0;
	if (ctrl)
		hwnd = (HWND)ctrl->GetHandle();
	const WChar *wmessage = Text::StrToWCharNew(message.v);
	const WChar *wtitle = Text::StrToWCharNew(title.v);
	Bool ret = (MessageBoxW(hwnd, wmessage, wtitle, MB_YESNO) == IDYES);
	Text::StrDelNew(wmessage);
	Text::StrDelNew(wtitle);
	return ret;
}
