#include "Stdafx.h"
#include "UI/GUIRadioButton.h"

UI::GUIRadioButton::GUIRadioButton(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIRadioButton::~GUIRadioButton()
{
}

Text::CStringNN UI::GUIRadioButton::GetObjectClass() const
{
	return CSTR("RadioButton");
}

void UI::GUIRadioButton::EventSelectedChange(Bool newState)
{
	UOSInt i = this->selectedChangeHdlrs.GetCount();
	while (i-- > 0)
	{
		this->selectedChangeHdlrs.GetItem(i)(this->selectedChangeObjs.GetItem(i), newState);
	}
}

void UI::GUIRadioButton::HandleSelectedChange(SelectedChangeHandler hdlr, void *userObj)
{
	this->selectedChangeHdlrs.Add(hdlr);
	this->selectedChangeObjs.Add(userObj);
}
