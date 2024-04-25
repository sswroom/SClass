#include "Stdafx.h"
#include "UI/GUITrackBar.h"

UI::GUITrackBar::GUITrackBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : GUIControl(ui, parent)
{
}

UI::GUITrackBar::~GUITrackBar()
{
}

Text::CStringNN UI::GUITrackBar::GetObjectClass() const
{
	return CSTR("TrackBar");
}

void UI::GUITrackBar::EventScrolled(UOSInt scrollPos)
{
	UOSInt i;
	i = this->scrollHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<ScrollEvent> cb = this->scrollHandlers.GetItem(i);
		cb.func(cb.userObj, scrollPos);
	}
}

void UI::GUITrackBar::HandleScrolled(ScrollEvent hdlr, AnyType userObj)
{
	this->scrollHandlers.Add({hdlr, userObj});
}
