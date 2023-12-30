#include "Stdafx.h"
#include "UI/GUIDateTimePicker.h"

UI::GUIDateTimePicker::GUIDateTimePicker(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : GUIControl(ui, parent)
{
}

UI::GUIDateTimePicker::~GUIDateTimePicker()
{
}

Text::CStringNN UI::GUIDateTimePicker::GetObjectClass() const
{
	return CSTR("DateTimePicker");
}

void UI::GUIDateTimePicker::EventDateChange(NotNullPtr<Data::DateTime> newDate)
{
	UOSInt i = this->dateChangedHdlrs.GetCount();
	while (i-- > 0)
	{
		this->dateChangedHdlrs.GetItem(i)(this->dateChangedObjs.GetItem(i), newDate);
	}
}

void UI::GUIDateTimePicker::HandleDateChange(DateChangedHandler hdlr, void *obj)
{
	this->dateChangedHdlrs.Add(hdlr);
	this->dateChangedObjs.Add(obj);
}
