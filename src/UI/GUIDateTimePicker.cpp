#include "Stdafx.h"
#include "UI/GUIDateTimePicker.h"

UI::GUIDateTimePicker::GUIDateTimePicker(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : GUIControl(ui, parent)
{
}

UI::GUIDateTimePicker::~GUIDateTimePicker()
{
}

Text::CStringNN UI::GUIDateTimePicker::GetObjectClass() const
{
	return CSTR("DateTimePicker");
}

void UI::GUIDateTimePicker::EventDateChange(NN<Data::DateTime> newDate)
{
	UIntOS i = this->dateChangedHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<DateChangedHandler> cb = this->dateChangedHdlrs.GetItem(i);
		cb.func(cb.userObj, newDate);
	}
}

void UI::GUIDateTimePicker::HandleDateChange(DateChangedHandler hdlr, AnyType obj)
{
	this->dateChangedHdlrs.Add({hdlr, obj});
}
