#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayList.hpp"
#include "UI/JavaUI/JUIRadioButton.h"

void UI::JavaUI::JUIRadioButton::ChangeSelected(Bool selVal)
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

UI::JavaUI::JUIRadioButton::JUIRadioButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool selected) : UI::GUIRadioButton(ui, parent)
{
	this->selected = selected;
}

UI::JavaUI::JUIRadioButton::~JUIRadioButton()
{
}

IntOS UI::JavaUI::JUIRadioButton::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::JavaUI::JUIRadioButton::IsSelected()
{
	return this->selected;
}

void UI::JavaUI::JUIRadioButton::Select()
{
}
