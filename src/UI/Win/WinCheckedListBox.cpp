#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/Win/WinCheckedListBox.h"
#include <windows.h>
#include <commctrl.h>

UI::Win::WinCheckedListBox::WinCheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUICheckedListBox(ui, parent)
{
	SendMessage((HWND)this->hwnd.OrNull(), LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_CHECKBOXES, LVS_EX_CHECKBOXES);
}

UI::Win::WinCheckedListBox::~WinCheckedListBox()
{
}

Bool UI::Win::WinCheckedListBox::GetItemChecked(UOSInt index)
{
	return ListView_GetCheckState((HWND)this->hwnd.OrNull(), index) != 0;
}

void UI::Win::WinCheckedListBox::SetItemChecked(UOSInt index, Bool isChecked)
{
	ListView_SetCheckState((HWND)this->hwnd.OrNull(), index, isChecked?TRUE:FALSE);
}

OSInt UI::Win::WinCheckedListBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
