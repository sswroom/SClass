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
		this->btnFocusLostHandlers.GetItem(i)(this->btnFocusLostHandlersObjs.GetItem(i));
	}
}

void UI::GUIButton::EventButtonClick()
{
	UOSInt i;
	i = this->btnClkHandlers.GetCount();
	while (i-- > 0)
	{
		this->btnClkHandlers.GetItem(i)(this->btnClkHandlersObjs.GetItem(i));
	}
}

void UI::GUIButton::EventButtonDown()
{
	UOSInt i;
	i = this->btnUpDownHandlers.GetCount();
	while (i-- > 0)
	{
		this->btnUpDownHandlers.GetItem(i)(this->btnUpDownHandlersObjs.GetItem(i), true);
	}
}

void UI::GUIButton::EventButtonUp()
{
	UOSInt i;
	i = this->btnUpDownHandlers.GetCount();
	while (i-- > 0)
	{
		this->btnUpDownHandlers.GetItem(i)(this->btnUpDownHandlersObjs.GetItem(i), false);
	}
}

void UI::GUIButton::HandleFocusLost(UIEvent handler, void *userObj)
{
	this->btnFocusLostHandlers.Add(handler);
	this->btnFocusLostHandlersObjs.Add(userObj);
}

void UI::GUIButton::HandleButtonClick(UIEvent handler, void *userObj)
{
	this->btnClkHandlers.Add(handler);
	this->btnClkHandlersObjs.Add(userObj);
}

void UI::GUIButton::HandleButtonUpDown(UpDownEvent handler, void *userObj)
{
	this->btnUpDownHandlers.Add(handler);
	this->btnUpDownHandlersObjs.Add(userObj);
}
