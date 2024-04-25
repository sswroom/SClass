#include "Stdafx.h"
#include "UI/GUIListBox.h"

UI::GUIListBox::GUIListBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
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
		Data::CallbackStorage<UI::UIEvent> cb = this->selChgHdlrs.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUIListBox::EventDoubleClick()
{
	UOSInt i = this->dblClickHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->dblClickHdlrs.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUIListBox::EventRightClick(Math::Coord2D<OSInt> pos)
{
	UOSInt i = this->rightClickHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<MouseEventHandler> cb = this->rightClickHdlrs.GetItem(i);
		cb.func(cb.userObj, pos, UI::GUIControl::MBTN_RIGHT);
	}
}

void UI::GUIListBox::HandleSelectionChange(UI::UIEvent hdlr, AnyType userObj)
{
	this->selChgHdlrs.Add({hdlr, userObj});
}

void UI::GUIListBox::HandleDoubleClicked(UI::UIEvent hdlr, AnyType userObj)
{
	this->dblClickHdlrs.Add({hdlr, userObj});
}

void UI::GUIListBox::HandleRightClicked(UI::GUIControl::MouseEventHandler hdlr, AnyType userObj)
{
	this->rightClickHdlrs.Add({hdlr, userObj});
}
