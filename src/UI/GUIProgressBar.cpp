#include "Stdafx.h"
#include "UI/GUIProgressBar.h"

UI::GUIProgressBar::GUIProgressBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIProgressBar::~GUIProgressBar()
{
}

Text::CStringNN UI::GUIProgressBar::GetObjectClass() const
{
	return CSTR("ProgressBar");
}

