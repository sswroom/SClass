#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/JavaUI/JUICheckBox.h"

UI::JavaUI::JUICheckBox::JUICheckBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked) : UI::GUICheckBox(ui, parent)
{
}

UI::JavaUI::JUICheckBox::~JUICheckBox()
{
}

OSInt UI::JavaUI::JUICheckBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::JavaUI::JUICheckBox::IsChecked()
{
	return false;
}

void UI::JavaUI::JUICheckBox::SetChecked(Bool checked)
{
}


