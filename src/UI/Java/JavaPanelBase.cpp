#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIPanel.h"
#include "UI/Java/JavaPanelBase.h"

UI::Java::JavaPanelBase::JavaPanelBase(NN<UI::GUIPanel> master, NN<UI::GUICore> ui, ControlHandle *parentHWnd)
{
	this->master = master;
}

UI::Java::JavaPanelBase::JavaPanelBase(NN<UI::GUIPanel> master, NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent)
{
	this->master = master;
	parent->AddChild(this->master);
	this->master->Show();
}

UI::Java::JavaPanelBase::~JavaPanelBase()
{
}

Math::Coord2DDbl UI::Java::JavaPanelBase::GetClientOfst()
{
	return Math::Coord2DDbl(0, 0);
}

Math::Size2DDbl UI::Java::JavaPanelBase::GetClientSize()
{
	return this->master->GetSize();
}

void UI::Java::JavaPanelBase::SetMinSize(Int32 minW, Int32 minH)
{
}
