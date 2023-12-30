#include "Stdafx.h"
#include "UI/GUIProgressBar.h"

UI::GUIProgressBar::GUIProgressBar(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIProgressBar::~GUIProgressBar()
{
}

Text::CStringNN UI::GUIProgressBar::GetObjectClass() const
{
	return CSTR("ProgressBar");
}

