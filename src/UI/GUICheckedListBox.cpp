#include "Stdafx.h"
#include "UI/GUICheckedListBox.h"

UI::GUICheckedListBox::GUICheckedListBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIListView(ui, parent, UI::GUIListView::LVSTYLE_SMALLICON, 1)
{
}

UI::GUICheckedListBox::~GUICheckedListBox()
{
}

Text::CStringNN UI::GUICheckedListBox::GetObjectClass() const
{
	return CSTR("CheckedListBox");
}