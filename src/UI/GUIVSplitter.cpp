#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIVSplitter.h"

UI::GUIVSplitter::GUIVSplitter(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIVSplitter::~GUIVSplitter()
{
}

Text::CStringNN UI::GUIVSplitter::GetObjectClass() const
{
	return CSTR("VSplitter");
}
