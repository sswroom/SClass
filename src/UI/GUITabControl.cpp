#include "Stdafx.h"
#include "UI/GUITabControl.h"

UI::GUITabControl::GUITabControl(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : GUIControl(ui, parent)
{
}

UI::GUITabControl::~GUITabControl()
{
}

Text::CStringNN UI::GUITabControl::GetObjectClass() const
{
	return CSTR("TabControl");
}

void *UI::GUITabControl::GetTabPageFont()
{
	return this->GetFont();
}

void UI::GUITabControl::HandleSelChanged(UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs.Add(hdlr);
	this->selChgObjs.Add(userObj);
}

void UI::GUITabControl::EventSelChange()
{
	UOSInt i;
	i = this->selChgHdlrs.GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs.GetItem(i)(this->selChgObjs.GetItem(i));
	}
}
