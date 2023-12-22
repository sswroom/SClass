#include "Stdafx.h"
#include "UI/GUIListBox.h"

UI::GUIListBox::GUIListBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIListBox::~GUIListBox()
{
}

Text::CStringNN UI::GUIListBox::GetObjectClass() const
{
	return CSTR("ListBox");
}

void UI::GUIListBox::EventSelectionChange()
{
	UOSInt i = this->selChgHdlrs.GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs.GetItem(i)(this->selChgObjs.GetItem(i));
	}
}

void UI::GUIListBox::EventDoubleClick()
{
	UOSInt i = this->dblClickHdlrs.GetCount();
	while (i-- > 0)
	{
		this->dblClickHdlrs.GetItem(i)(this->dblClickObjs.GetItem(i));
	}
}

void UI::GUIListBox::EventRightClick(Math::Coord2D<OSInt> pos)
{
	UOSInt i = this->rightClickHdlrs.GetCount();
	while (i-- > 0)
	{
		this->rightClickHdlrs.GetItem(i)(this->rightClickObjs.GetItem(i), pos, UI::GUIControl::MBTN_RIGHT);
	}
}

void UI::GUIListBox::HandleSelectionChange(UI::UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs.Add(hdlr);
	this->selChgObjs.Add(userObj);
}

void UI::GUIListBox::HandleDoubleClicked(UI::UIEvent hdlr, void *userObj)
{
	this->dblClickHdlrs.Add(hdlr);
	this->dblClickObjs.Add(userObj);
}

void UI::GUIListBox::HandleRightClicked(UI::GUIControl::MouseEventHandler hdlr, void *userObj)
{
	this->rightClickHdlrs.Add(hdlr);
	this->rightClickObjs.Add(userObj);
}
