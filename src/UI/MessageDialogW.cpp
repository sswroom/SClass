#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/MessageDialog.h"
#include <windows.h>

void UI::MessageDialog::ShowDialog(const UTF8Char *message, const UTF8Char *title, UI::GUIControl *ctrl)
{
	HWND hwnd = 0;
	if (ctrl)
		hwnd = (HWND)ctrl->GetHandle();
	const WChar *wmessage = Text::StrToWCharNew(message);
	const WChar *wtitle = Text::StrToWCharNew(title);
	MessageBoxW(hwnd, wmessage, wtitle, MB_OK);
	Text::StrDelNew(wmessage);
	Text::StrDelNew(wtitle);
}

Bool UI::MessageDialog::ShowYesNoDialog(const UTF8Char *message, const UTF8Char *title, UI::GUIControl *ctrl)
{
	HWND hwnd = 0;
	if (ctrl)
		hwnd = (HWND)ctrl->GetHandle();
	const WChar *wmessage = Text::StrToWCharNew(message);
	const WChar *wtitle = Text::StrToWCharNew(title);
	Bool ret = (MessageBoxW(hwnd, wmessage, wtitle, MB_YESNO) == IDYES);
	Text::StrDelNew(wmessage);
	Text::StrDelNew(wtitle);
	return ret;
}
