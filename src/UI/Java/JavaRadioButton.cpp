#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayList.h"
#include "UI/Java/JavaRadioButton.h"

void UI::Java::JavaRadioButton::ChangeSelected(Bool selVal)
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

UI::Java::JavaRadioButton::JavaRadioButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool selected) : UI::GUIRadioButton(ui, parent)
{
	this->selected = selected;
}

UI::Java::JavaRadioButton::~JavaRadioButton()
{
}

OSInt UI::Java::JavaRadioButton::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::Java::JavaRadioButton::IsSelected()
{
	return this->selected;
}

void UI::Java::JavaRadioButton::Select()
{
}
