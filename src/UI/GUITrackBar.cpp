#include "Stdafx.h"
#include "UI/GUITrackBar.h"

UI::GUITrackBar::GUITrackBar(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : GUIControl(ui, parent)
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
		this->scrollHandlers.GetItem(i)(this->scrollHandlersObj.GetItem(i), scrollPos);
	}
}

void UI::GUITrackBar::HandleScrolled(ScrollEvent hdlr, void *userObj)
{
	this->scrollHandlers.Add(hdlr);
	this->scrollHandlersObj.Add(userObj);
}
