#include "Stdafx.h"
#include "UI/GUILabel.h"

UI::GUILabel::GUILabel(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUILabel::~GUILabel()
{
}

Text::CStringNN UI::GUILabel::GetObjectClass() const
{
	return CSTR("Label");
}
