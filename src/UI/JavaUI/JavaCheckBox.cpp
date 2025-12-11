#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/JavaUI/JavaCheckBox.h"

UI::JavaUI::JavaCheckBox::JavaCheckBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked) : UI::GUICheckBox(ui, parent)
{
}

UI::JavaUI::JavaCheckBox::~JavaCheckBox()
{
}

OSInt UI::JavaUI::JavaCheckBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::JavaUI::JavaCheckBox::IsChecked()
{
	return false;
}

void UI::JavaUI::JavaCheckBox::SetChecked(Bool checked)
{
}


