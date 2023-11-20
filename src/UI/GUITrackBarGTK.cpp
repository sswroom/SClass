#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Math/Math.h"
#include "UI/GUITrackBar.h"

#include <gtk/gtk.h>

gboolean GUITrackBar_ValueChanged(void *window, void *userObj)
{
	UI::GUITrackBar *me = (UI::GUITrackBar*)userObj;
	me->EventScrolled();
	return FALSE;
}

void UI::GUITrackBar::EventScrolled()
{
	UOSInt i;
	UOSInt pos;
	pos = (UOSInt)Double2OSInt(gtk_range_get_value((GtkRange*)this->hwnd));
	i = this->scrollHandlers.GetCount();
	while (i-- > 0)
	{
		this->scrollHandlers.GetItem(i)(this->scrollHandlersObj.GetItem(i), pos);
	}
}

UI::GUITrackBar::GUITrackBar(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal) : UI::GUIControl(ui, parent)
{
	if (minVal >= maxVal)
	{
		maxVal = minVal + 1;
	}
	this->hwnd = (ControlHandle*)gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, UOSInt2Double(minVal), UOSInt2Double(maxVal), 1);
	gtk_scale_set_draw_value((GtkScale*)this->hwnd, false);
	gtk_range_set_value((GtkRange*)this->hwnd, UOSInt2Double(currVal));
	g_signal_connect((GtkRange*)this->hwnd, "value-changed", G_CALLBACK(GUITrackBar_ValueChanged), this);
	parent->AddChild(this);
	this->Show();
}

UI::GUITrackBar::~GUITrackBar()
{
}

Text::CStringNN UI::GUITrackBar::GetObjectClass() const
{
	return CSTR("TrackBar");
}

OSInt UI::GUITrackBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUITrackBar::SetPos(UOSInt pos)
{
	gtk_range_set_value((GtkRange*)this->hwnd, UOSInt2Double(pos));
}

void UI::GUITrackBar::SetRange(UOSInt minVal, UOSInt maxVal)
{
	gtk_range_set_range((GtkRange*)this->hwnd, UOSInt2Double(minVal), UOSInt2Double(maxVal));
}

UOSInt UI::GUITrackBar::GetPos()
{
	return (UOSInt)Double2OSInt(gtk_range_get_value((GtkRange*)this->hwnd));
}

void UI::GUITrackBar::HandleScrolled(ScrollEvent hdlr, void *userObj)
{
	this->scrollHandlers.Add(hdlr);
	this->scrollHandlersObj.Add(userObj);
}
