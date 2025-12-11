#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayList.hpp"
#include "UI/JavaUI/JavaRadioButton.h"

void UI::JavaUI::JavaRadioButton::ChangeSelected(Bool selVal)
{
	if (this->selected == selVal)
		return;
	if (selVal)
	{
		this->selected = true;
	}
	else
	{
		this->selected = false;
	}
	this->EventSelectedChange(selVal);
}

UI::JavaUI::JavaRadioButton::JavaRadioButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool selected) : UI::GUIRadioButton(ui, parent)
{
	this->selected = selected;
}

UI::JavaUI::JavaRadioButton::~JavaRadioButton()
{
}

OSInt UI::JavaUI::JavaRadioButton::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::JavaUI::JavaRadioButton::IsSelected()
{
	return this->selected;
}

void UI::JavaUI::JavaRadioButton::Select()
{
}
