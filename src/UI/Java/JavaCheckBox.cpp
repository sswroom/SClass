#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/Java/JavaCheckBox.h"

UI::Java::JavaCheckBox::JavaCheckBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked) : UI::GUICheckBox(ui, parent)
{
}

UI::Java::JavaCheckBox::~JavaCheckBox()
{
}

OSInt UI::Java::JavaCheckBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::Java::JavaCheckBox::IsChecked()
{
	return false;
}

void UI::Java::JavaCheckBox::SetChecked(Bool checked)
{
}


