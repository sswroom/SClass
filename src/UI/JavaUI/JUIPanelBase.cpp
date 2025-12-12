#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIPanel.h"
#include "UI/JavaUI/JUIPanelBase.h"

UI::JavaUI::JUIPanelBase::JUIPanelBase(NN<UI::GUIPanel> master, NN<UI::GUICore> ui, Optional<ControlHandle> parentHWnd)
{
	this->master = master;
}

UI::JavaUI::JUIPanelBase::JUIPanelBase(NN<UI::GUIPanel> master, NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent)
{
	this->master = master;
	parent->AddChild(this->master);
	this->master->Show();
}

UI::JavaUI::JUIPanelBase::~JUIPanelBase()
{
}

Math::Coord2DDbl UI::JavaUI::JUIPanelBase::GetClientOfst()
{
	return Math::Coord2DDbl(0, 0);
}

Math::Size2DDbl UI::JavaUI::JUIPanelBase::GetClientSize()
{
	return this->master->GetSize();
}

void UI::JavaUI::JUIPanelBase::SetMinSize(Int32 minW, Int32 minH)
{
}
