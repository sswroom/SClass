#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIPopupMenu.h"

#include <windows.h>

UI::GUIPopupMenu::GUIPopupMenu() : UI::GUIMenu(true)
{
}

void UI::GUIPopupMenu::ShowMenu(NotNullPtr<UI::GUIControl> ctrl, Math::Coord2D<OSInt> scnPos)
{
//	UInt32 err;
	NotNullPtr<UI::GUIForm> frm;
	if (frm.Set(ctrl->GetRootForm()))
		ctrl = frm;
	if (TrackPopupMenu((HMENU)this->hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, (Int32)scnPos.x, (Int32)scnPos.y, 0, (HWND)ctrl->GetHandle(), 0) == 0)
	{
//		err = GetLastError();
	}
}
