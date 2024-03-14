#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUIButton.h"

UI::GUIButton::GUIButton(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIButton::~GUIButton()
{
}

Text::CStringNN UI::GUIButton::GetObjectClass() const
{
	return CSTR("BUTTON");
}

void UI::GUIButton::EventFocus()
{
}

void UI::GUIButton::EventFocusLost()
{
	UOSInt i;
	i = this->btnFocusLostHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UIEvent> cb = this->btnFocusLostHandlers.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUIButton::EventButtonClick()
{
	UOSInt i;
	i = this->btnClkHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UIEvent> cb = this->btnClkHandlers.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUIButton::EventButtonDown()
{
	UOSInt i;
	i = this->btnUpDownHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UpDownEvent> cb = this->btnUpDownHandlers.GetItem(i);
		cb.func(cb.userObj, true);
	}
}

void UI::GUIButton::EventButtonUp()
{
	UOSInt i;
	i = this->btnUpDownHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UpDownEvent> cb = this->btnUpDownHandlers.GetItem(i);
		cb.func(cb.userObj, false);
	}
}

void UI::GUIButton::HandleFocusLost(UIEvent handler, AnyType userObj)
{
	this->btnFocusLostHandlers.Add({handler, userObj});
}

void UI::GUIButton::HandleButtonClick(UIEvent handler, AnyType userObj)
{
	this->btnClkHandlers.Add({handler, userObj});
}

void UI::GUIButton::HandleButtonUpDown(UpDownEvent handler, AnyType userObj)
{
	this->btnUpDownHandlers.Add({handler, userObj});
}
