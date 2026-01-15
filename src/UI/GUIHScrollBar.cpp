#include "Stdafx.h"
#include "UI/GUIHScrollBar.h"

UI::GUIHScrollBar::GUIHScrollBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIHScrollBar::~GUIHScrollBar()
{
}

Text::CStringNN UI::GUIHScrollBar::GetObjectClass() const
{
	return CSTR("HScrollBar");
}

void UI::GUIHScrollBar::EventPosChanged(UIntOS newPos)
{
	UIntOS i;
	i = this->posChgHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<PosChgEvent> cb = this->posChgHdlrs.GetItem(i);
		cb.func(cb.userObj, newPos);
	}
}

void UI::GUIHScrollBar::HandlePosChanged(PosChgEvent hdlr, AnyType userObj)
{
	this->posChgHdlrs.Add({hdlr, userObj});
}
