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
	Int32 pos;
	pos = Math::Double2Int32(gtk_range_get_value((GtkRange*)this->hwnd));
	i = this->scrollHandlers->GetCount();
	while (i-- > 0)
	{
		this->scrollHandlers->GetItem(i)(this->scrollHandlersObj->GetItem(i), pos);
	}
}

UI::GUITrackBar::GUITrackBar(UI::GUICore *ui, UI::GUIClientControl *parent, Int32 minVal, Int32 maxVal, Int32 currVal) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->scrollHandlers, Data::ArrayList<ScrollEvent>());
	NEW_CLASS(this->scrollHandlersObj, Data::ArrayList<void *>());

	if (minVal >= maxVal)
	{
		maxVal = minVal + 1;
	}
	this->hwnd = (ControlHandle*)gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, minVal, maxVal, 1);
	gtk_scale_set_draw_value((GtkScale*)this->hwnd, false);
	gtk_range_set_value((GtkRange*)this->hwnd, currVal);
	g_signal_connect((GtkRange*)this->hwnd, "value-changed", G_CALLBACK(GUITrackBar_ValueChanged), this);
	parent->AddChild(this);
	this->Show();
}

UI::GUITrackBar::~GUITrackBar()
{
	DEL_CLASS(this->scrollHandlersObj);
	DEL_CLASS(this->scrollHandlers);
}

const UTF8Char *UI::GUITrackBar::GetObjectClass()
{
	return (const UTF8Char*)"TrackBar";
}

OSInt UI::GUITrackBar::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUITrackBar::SetPos(Int32 pos)
{
	gtk_range_set_value((GtkRange*)this->hwnd, pos);
}

void UI::GUITrackBar::SetRange(Int32 minVal, Int32 maxVal)
{
	gtk_range_set_range((GtkRange*)this->hwnd, minVal, maxVal);
}

Int32 UI::GUITrackBar::GetPos()
{
	return Math::Double2Int32(gtk_range_get_value((GtkRange*)this->hwnd));
}

void UI::GUITrackBar::HandleScrolled(ScrollEvent hdlr, void *userObj)
{
	this->scrollHandlers->Add(hdlr);
	this->scrollHandlersObj->Add(userObj);
}
