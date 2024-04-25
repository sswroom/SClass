#include "Stdafx.h"
#include "UI/GUICheckedListBox.h"

UI::GUICheckedListBox::GUICheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIListView(ui, parent, UI::ListViewStyle::SmallIcon, 1)
{
}

UI::GUICheckedListBox::~GUICheckedListBox()
{
}

Text::CStringNN UI::GUICheckedListBox::GetObjectClass() const
{
	return CSTR("CheckedListBox");
}