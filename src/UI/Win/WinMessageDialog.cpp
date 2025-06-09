#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Win/WinMessageDialog.h"
#include <windows.h>

void UI::Win::WinMessageDialog::ShowOK(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	HWND hwnd = 0;
	NN<GUIControl> nnctrl;
	if (ctrl.SetTo(nnctrl))
		hwnd = (HWND)nnctrl->GetHandle().OrNull();
	UnsafeArray<const WChar> wmessage = Text::StrToWCharNew(message.v);
	UnsafeArray<const WChar> wtitle = Text::StrToWCharNew(title.v);
	MessageBoxW(hwnd, wmessage.Ptr(), wtitle.Ptr(), MB_OK);
	Text::StrDelNew(wmessage);
	Text::StrDelNew(wtitle);
}

Bool UI::Win::WinMessageDialog::ShowYesNo(Text::CStringNN message, Text::CStringNN title, Optional<UI::GUIControl> ctrl)
{
	HWND hwnd = 0;
	NN<GUIControl> nnctrl;
	if (ctrl.SetTo(nnctrl))
		hwnd = (HWND)nnctrl->GetHandle().OrNull();
	UnsafeArray<const WChar> wmessage = Text::StrToWCharNew(message.v);
	UnsafeArray<const WChar> wtitle = Text::StrToWCharNew(title.v);
	Bool ret = (MessageBoxW(hwnd, wmessage.Ptr(), wtitle.Ptr(), MB_YESNO) == IDYES);
	Text::StrDelNew(wmessage);
	Text::StrDelNew(wtitle);
	return ret;
}
