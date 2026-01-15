#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "UI/GTK/GTKTrackBar.h"

gboolean UI::GTK::GTKTrackBar::SignalValueChanged(void *window, void *userObj)
{
	UI::GUITrackBar *me = (UI::GUITrackBar*)userObj;
	me->EventScrolled(me->GetPos());
	return FALSE;
}

UI::GTK::GTKTrackBar::GTKTrackBar(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, UIntOS minVal, UIntOS maxVal, UIntOS currVal) : UI::GUITrackBar(ui, parent)
{
	if (minVal >= maxVal)
	{
		maxVal = minVal + 1;
	}
	this->hwnd = (ControlHandle*)gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, UIntOS2Double(minVal), UIntOS2Double(maxVal), 1);
	gtk_scale_set_draw_value((GtkScale*)this->hwnd.OrNull(), false);
	gtk_range_set_value((GtkRange*)this->hwnd.OrNull(), UIntOS2Double(currVal));
	g_signal_connect((GtkRange*)this->hwnd.OrNull(), "value-changed", G_CALLBACK(SignalValueChanged), this);
	parent->AddChild(*this);
	this->Show();
}

UI::GTK::GTKTrackBar::~GTKTrackBar()
{
}

IntOS UI::GTK::GTKTrackBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKTrackBar::SetPos(UIntOS pos)
{
	gtk_range_set_value((GtkRange*)this->hwnd.OrNull(), UIntOS2Double(pos));
}

void UI::GTK::GTKTrackBar::SetRange(UIntOS minVal, UIntOS maxVal)
{
	gtk_range_set_range((GtkRange*)this->hwnd.OrNull(), UIntOS2Double(minVal), UIntOS2Double(maxVal));
}

UIntOS UI::GTK::GTKTrackBar::GetPos()
{
	return (UIntOS)Double2IntOS(gtk_range_get_value((GtkRange*)this->hwnd.OrNull()));
}
