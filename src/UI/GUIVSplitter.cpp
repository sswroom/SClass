#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIVSplitter.h"

UI::GUIVSplitter::GUIVSplitter(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIVSplitter::~GUIVSplitter()
{
}

Text::CStringNN UI::GUIVSplitter::GetObjectClass() const
{
	return CSTR("VSplitter");
}
