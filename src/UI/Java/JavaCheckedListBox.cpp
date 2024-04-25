#include "Stdafx.h"
#include "UI/Java/JavaCheckedListBox.h"

UI::Java::JavaCheckedListBox::JavaCheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUICheckedListBox(ui, parent)
{
}

UI::Java::JavaCheckedListBox::~JavaCheckedListBox()
{
}

Bool UI::Java::JavaCheckedListBox::GetItemChecked(UOSInt index)
{
	return false;
}

void UI::Java::JavaCheckedListBox::SetItemChecked(UOSInt index, Bool isChecked)
{
}

OSInt UI::Java::JavaCheckedListBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
