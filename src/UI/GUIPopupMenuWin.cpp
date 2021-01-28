#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIPopupMenu.h"

#include <windows.h>

UI::GUIPopupMenu::GUIPopupMenu() : UI::GUIMenu(true)
{
}

void UI::GUIPopupMenu::ShowMenu(UI::GUIControl *ctrl, OSInt x, OSInt y)
{
//	UInt32 err;
	UI::GUIForm *frm = ctrl->GetRootForm();
	if (frm)
		ctrl = frm;
	if (TrackPopupMenu((HMENU)this->hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, (Int32)x, (Int32)y, 0, (HWND)ctrl->GetHandle(), 0) == 0)
	{
//		err = GetLastError();
	}
}
