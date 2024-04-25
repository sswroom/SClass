#include "Stdafx.h"
#include "UI/GUICheckBox.h"

UI::GUICheckBox::GUICheckBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUICheckBox::~GUICheckBox()
{
}

Text::CStringNN UI::GUICheckBox::GetObjectClass() const
{
	return CSTR("CheckBox");
}

void UI::GUICheckBox::EventCheckedChange(Bool newState)
{
	UOSInt i = this->checkedChangeHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<CheckedChangeHandler> cb = this->checkedChangeHdlrs.GetItem(i);
		cb.func(cb.userObj, newState);
	}
}

void UI::GUICheckBox::HandleCheckedChange(UI::GUICheckBox::CheckedChangeHandler hdlr, AnyType obj)
{
	this->checkedChangeHdlrs.Add({hdlr, obj});
}
