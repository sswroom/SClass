#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIHSplitter.h"

UI::GUIHSplitter::GUIHSplitter(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIHSplitter::~GUIHSplitter()
{
}

Text::CStringNN UI::GUIHSplitter::GetObjectClass() const
{
	return CSTR("HSplitter");
}
