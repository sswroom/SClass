#include "Stdafx.h"
#include "UI/GUIHScrollBar.h"

UI::GUIHScrollBar::GUIHScrollBar(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
}

UI::GUIHScrollBar::~GUIHScrollBar()
{
}

Text::CStringNN UI::GUIHScrollBar::GetObjectClass() const
{
	return CSTR("HScrollBar");
}

void UI::GUIHScrollBar::EventPosChanged(UOSInt newPos)
{
	UOSInt i;
	i = this->posChgHdlrs.GetCount();
	while (i-- > 0)
	{
		this->posChgHdlrs.GetItem(i)(this->posChgObjs.GetItem(i), newPos);
	}
}

void UI::GUIHScrollBar::HandlePosChanged(PosChgEvent hdlr, void *userObj)
{
	this->posChgHdlrs.Add(hdlr);
	this->posChgObjs.Add(userObj);
}
