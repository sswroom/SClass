#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Win/WinMessageDialog.h"
#include <windows.h>

void UI::Win::WinMessageDialog::ShowOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	HWND hwnd = 0;
	NotNullPtr<GUIControl> nnctrl;
	if (ctrl.SetTo(nnctrl))
		hwnd = (HWND)nnctrl->GetHandle();
	const WChar *wmessage = Text::StrToWCharNew(message.v);
	const WChar *wtitle = Text::StrToWCharNew(title.v);
	MessageBoxW(hwnd, wmessage, wtitle, MB_OK);
	Text::StrDelNew(wmessage);
	Text::StrDelNew(wtitle);
}

Bool UI::Win::WinMessageDialog::ShowYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	HWND hwnd = 0;
	NotNullPtr<GUIControl> nnctrl;
	if (ctrl.SetTo(nnctrl))
		hwnd = (HWND)nnctrl->GetHandle();
	const WChar *wmessage = Text::StrToWCharNew(message.v);
	const WChar *wtitle = Text::StrToWCharNew(title.v);
	Bool ret = (MessageBoxW(hwnd, wmessage, wtitle, MB_YESNO) == IDYES);
	Text::StrDelNew(wmessage);
	Text::StrDelNew(wtitle);
	return ret;
}
