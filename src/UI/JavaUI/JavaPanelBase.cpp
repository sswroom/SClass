#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIPanel.h"
#include "UI/JavaUI/JavaPanelBase.h"

UI::JavaUI::JavaPanelBase::JavaPanelBase(NN<UI::GUIPanel> master, NN<UI::GUICore> ui, Optional<ControlHandle> parentHWnd)
{
	this->master = master;
}

UI::JavaUI::JavaPanelBase::JavaPanelBase(NN<UI::GUIPanel> master, NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent)
{
	this->master = master;
	parent->AddChild(this->master);
	this->master->Show();
}

UI::JavaUI::JavaPanelBase::~JavaPanelBase()
{
}

Math::Coord2DDbl UI::JavaUI::JavaPanelBase::GetClientOfst()
{
	return Math::Coord2DDbl(0, 0);
}

Math::Size2DDbl UI::JavaUI::JavaPanelBase::GetClientSize()
{
	return this->master->GetSize();
}

void UI::JavaUI::JavaPanelBase::SetMinSize(Int32 minW, Int32 minH)
{
}
