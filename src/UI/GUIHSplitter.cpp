#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIHSplitter.h"

UI::GUIHSplitter::GUIHSplitter(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIHSplitter::~GUIHSplitter()
{
}

Text::CStringNN UI::GUIHSplitter::GetObjectClass() const
{
	return CSTR("HSplitter");
}
