#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUIPanel.h"
#include "UI/GTK/GTKPanelBase.h"
#include <gtk/gtk.h>

UI::GTK::GTKPanelBase::GTKPanelBase(NotNullPtr<UI::GUIPanel> master, NotNullPtr<UI::GUICore> ui, ControlHandle *parentHWnd)
{
	this->master = master;
	this->master->SetHandle((ControlHandle*)gtk_fixed_new());
}

UI::GTK::GTKPanelBase::GTKPanelBase(NotNullPtr<UI::GUIPanel> master, NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent)
{
	this->master = master;
	this->master->SetHandle((ControlHandle*)gtk_fixed_new());
	parent->AddChild(this->master);
	this->master->Show();
}

UI::GTK::GTKPanelBase::~GTKPanelBase()
{
}

Math::Coord2DDbl UI::GTK::GTKPanelBase::GetClientOfst()
{
	return Math::Coord2DDbl(0, 0);
}

Math::Size2DDbl UI::GTK::GTKPanelBase::GetClientSize()
{
	return this->master->GetSize();
}

void UI::GTK::GTKPanelBase::SetMinSize(Int32 minW, Int32 minH)
{
}
