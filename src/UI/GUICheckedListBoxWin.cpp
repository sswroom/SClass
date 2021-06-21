#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUICheckedListBox.h"
#include <windows.h>
#include <commctrl.h>

UI::GUICheckedListBox::GUICheckedListBox(GUICore *ui, UI::GUIClientControl *parent) : UI::GUIListView(ui, parent, UI::GUIListView::LVSTYLE_SMALLICON, 0)
{
	SendMessage((HWND)this->hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_CHECKBOXES, LVS_EX_CHECKBOXES);
}

UI::GUICheckedListBox::~GUICheckedListBox()
{
}

Bool UI::GUICheckedListBox::GetItemChecked(UOSInt index)
{
	return ListView_GetCheckState((HWND)this->hwnd, index) != 0;
}

void UI::GUICheckedListBox::SetItemChecked(UOSInt index, Bool isChecked)
{
	ListView_SetCheckState((HWND)this->hwnd, index, isChecked?TRUE:FALSE);
}

const UTF8Char *UI::GUICheckedListBox::GetObjectClass()
{
	return (const UTF8Char*)"CheckedListBox";
}

OSInt UI::GUICheckedListBox::OnNotify(Int32 code, void *lParam)
{
	return 0;
}
