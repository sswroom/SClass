#include "Stdafx.h"
#include "UI/JavaUI/JavaCheckedListBox.h"

UI::JavaUI::JavaCheckedListBox::JavaCheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUICheckedListBox(ui, parent)
{
}

UI::JavaUI::JavaCheckedListBox::~JavaCheckedListBox()
{
}

Bool UI::JavaUI::JavaCheckedListBox::GetItemChecked(UOSInt index)
{
	return false;
}

void UI::JavaUI::JavaCheckedListBox::SetItemChecked(UOSInt index, Bool isChecked)
{
}

OSInt UI::JavaUI::JavaCheckedListBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
