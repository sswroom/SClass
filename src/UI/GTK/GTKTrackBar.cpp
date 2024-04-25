#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Math/Math.h"
#include "UI/GTK/GTKTrackBar.h"

gboolean UI::GTK::GTKTrackBar::SignalValueChanged(void *window, void *userObj)
{
	UI::GUITrackBar *me = (UI::GUITrackBar*)userObj;
	me->EventScrolled(me->GetPos());
	return FALSE;
}

UI::GTK::GTKTrackBar::GTKTrackBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal) : UI::GUITrackBar(ui, parent)
{
	if (minVal >= maxVal)
	{
		maxVal = minVal + 1;
	}
	this->hwnd = (ControlHandle*)gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, UOSInt2Double(minVal), UOSInt2Double(maxVal), 1);
	gtk_scale_set_draw_value((GtkScale*)this->hwnd, false);
	gtk_range_set_value((GtkRange*)this->hwnd, UOSInt2Double(currVal));
	g_signal_connect((GtkRange*)this->hwnd, "value-changed", G_CALLBACK(SignalValueChanged), this);
	parent->AddChild(*this);
	this->Show();
}

UI::GTK::GTKTrackBar::~GTKTrackBar()
{
}

OSInt UI::GTK::GTKTrackBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKTrackBar::SetPos(UOSInt pos)
{
	gtk_range_set_value((GtkRange*)this->hwnd, UOSInt2Double(pos));
}

void UI::GTK::GTKTrackBar::SetRange(UOSInt minVal, UOSInt maxVal)
{
	gtk_range_set_range((GtkRange*)this->hwnd, UOSInt2Double(minVal), UOSInt2Double(maxVal));
}

UOSInt UI::GTK::GTKTrackBar::GetPos()
{
	return (UOSInt)Double2OSInt(gtk_range_get_value((GtkRange*)this->hwnd));
}
