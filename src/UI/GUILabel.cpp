#include "Stdafx.h"
#include "UI/GUILabel.h"

UI::GUILabel::GUILabel(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUILabel::~GUILabel()
{
}

Text::CStringNN UI::GUILabel::GetObjectClass() const
{
	return CSTR("Label");
}
