#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/JavaUI/JUIGroupBox.h"

UI::JavaUI::JUIGroupBox::JUIGroupBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN name) : GUIGroupBox(ui, parent)
{
}

UI::JavaUI::JUIGroupBox::~JUIGroupBox()
{
}

Math::Coord2DDbl UI::JavaUI::JUIGroupBox::GetClientOfst()
{
	return Math::Coord2DDbl{0, 0};
}

Math::Size2DDbl UI::JavaUI::JUIGroupBox::GetClientSize()
{
	return Math::Size2DDbl(0, 0);
}
