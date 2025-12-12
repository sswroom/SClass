#include "Stdafx.h"
#include "UI/JavaUI/JUICheckedListBox.h"

UI::JavaUI::JUICheckedListBox::JUICheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUICheckedListBox(ui, parent)
{
}

UI::JavaUI::JUICheckedListBox::~JUICheckedListBox()
{
}

Bool UI::JavaUI::JUICheckedListBox::GetItemChecked(UOSInt index)
{
	return false;
}

void UI::JavaUI::JUICheckedListBox::SetItemChecked(UOSInt index, Bool isChecked)
{
}

OSInt UI::JavaUI::JUICheckedListBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
