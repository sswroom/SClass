#include "Stdafx.h"
#include "UI/GUITextBox.h"

UI::GUITextBox::GUITextBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUITextBox::~GUITextBox()
{
}

Text::CStringNN UI::GUITextBox::GetObjectClass() const
{
	return CSTR("TextBox");
}

void UI::GUITextBox::EventTextChange()
{
	UOSInt i = this->txtChgHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->txtChgHdlrs.GetItem(i);
		cb.func(cb.userObj);
	}
}


Bool UI::GUITextBox::EventKeyDown(UInt32 osKey)
{
	Bool ret = false;
	UOSInt i = this->keyDownHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::KeyEvent> cb = this->keyDownHdlrs.GetItem(i);
		if ((ret = cb.func(cb.userObj, osKey)))
		{
			break;
		}
	}
	return ret;
}

void UI::GUITextBox::HandleTextChanged(UI::UIEvent hdlr, AnyType userObj)
{
	this->txtChgHdlrs.Add({hdlr, userObj});
}

void UI::GUITextBox::HandleKeyDown(UI::KeyEvent hdlr, AnyType userObj)
{
	this->keyDownHdlrs.Add({hdlr, userObj});
}
