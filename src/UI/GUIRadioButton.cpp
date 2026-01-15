#include "Stdafx.h"
#include "UI/GUIRadioButton.h"

UI::GUIRadioButton::GUIRadioButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
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
	UIntOS i = this->selectedChangeHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<SelectedChangeHandler> cb = this->selectedChangeHdlrs.GetItem(i);
		cb.func(cb.userObj, newState);
	}
}

void UI::GUIRadioButton::HandleSelectedChange(SelectedChangeHandler hdlr, AnyType userObj)
{
	this->selectedChangeHdlrs.Add({hdlr, userObj});
}
