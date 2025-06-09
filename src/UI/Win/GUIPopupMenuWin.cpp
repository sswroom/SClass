#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIPopupMenu.h"

#include <windows.h>

UI::GUIPopupMenu::GUIPopupMenu() : UI::GUIMenu(true)
{
}

void UI::GUIPopupMenu::ShowMenu(NN<UI::GUIControl> ctrl, Math::Coord2D<OSInt> scnPos)
{
//	UInt32 err;
	NN<UI::GUIForm> frm;
	if (ctrl->GetRootForm().SetTo(frm))
		ctrl = frm;
	if (TrackPopupMenu((HMENU)this->hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, (Int32)scnPos.x, (Int32)scnPos.y, 0, (HWND)ctrl->GetHandle().OrNull(), 0) == 0)
	{
//		err = GetLastError();
	}
}
