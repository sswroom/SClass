#include "Stdafx.h"
#include "UI/GUIPanel.h"

UI::GUIPanel::GUIPanel(NN<GUICore> ui, Optional<ControlHandle> parentHWnd) : UI::GUIClientControl(ui, 0)
{
	this->base = ui->NewPanelBase(*this, parentHWnd);
}

UI::GUIPanel::GUIPanel(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIClientControl(ui, parent)
{
	this->base = ui->NewPanelBase(*this, parent);
}

UI::GUIPanel::~GUIPanel()
{
	this->base.Delete();
}

Bool UI::GUIPanel::IsChildVisible()
{
	return true;
}

Math::Coord2DDbl UI::GUIPanel::GetClientOfst()
{
	return this->base->GetClientOfst();
}

Math::Size2DDbl UI::GUIPanel::GetClientSize()
{
	return this->base->GetClientSize();
}

Text::CStringNN UI::GUIPanel::GetObjectClass() const
{
	return CSTR("Panel");
}

OSInt UI::GUIPanel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIPanel::SetMinSize(Int32 minW, Int32 minH)
{
	this->base->SetMinSize(minW, minH);
}

void UI::GUIPanel::SetHandle(Optional<ControlHandle> hwnd)
{
	this->hwnd = hwnd;
}

NN<UI::GUIPanelBase> UI::GUIPanel::GetBase() const
{
	return this->base;
}
