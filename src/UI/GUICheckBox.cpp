#include "Stdafx.h"
#include "UI/GUICheckBox.h"

UI::GUICheckBox::GUICheckBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
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
		this->checkedChangeHdlrs.GetItem(i)(this->checkedChangeObjs.GetItem(i), newState);
	}
}

void UI::GUICheckBox::HandleCheckedChange(UI::GUICheckBox::CheckedChangeHandler hdlr, void *obj)
{
	this->checkedChangeHdlrs.Add(hdlr);
	this->checkedChangeObjs.Add(obj);
}
