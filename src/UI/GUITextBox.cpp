#include "Stdafx.h"
#include "UI/GUITextBox.h"

UI::GUITextBox::GUITextBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
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
		this->txtChgHdlrs.GetItem(i)(this->txtChgObjs.GetItem(i));
	}
}


Bool UI::GUITextBox::EventKeyDown(UInt32 osKey)
{
	Bool ret = false;
	UOSInt i = this->keyDownHdlrs.GetCount();
	while (i-- > 0)
	{
		if ((ret = this->keyDownHdlrs.GetItem(i)(this->keyDownObjs.GetItem(i), osKey)))
		{
			break;
		}
	}
	return ret;
}

void UI::GUITextBox::HandleTextChanged(UI::UIEvent hdlr, void *userObj)
{
	this->txtChgHdlrs.Add(hdlr);
	this->txtChgObjs.Add(userObj);
}

void UI::GUITextBox::HandleKeyDown(UI::KeyEvent hdlr, void *userObj)
{
	this->keyDownHdlrs.Add(hdlr);
	this->keyDownObjs.Add(userObj);
}
