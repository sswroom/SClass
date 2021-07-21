#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/Gtk/GtkTextView.h"
#include "UI/Gtk/GtkClientControl.h"
#include <gtk/gtk.h>

UI::Gtk::GtkTextView::GtkTextView(GtkUI *ui, GtkClientControl *parent, const UTF8Char *label) : UI::Gtk::GtkControl(parent)
{
	this->buff = gtk_text_buffer_new(0);
	gtk_text_buffer_set_text((GtkTextBuffer*)this->buff, (const Char*)label, -1);
	this->txtView = gtk_text_view_new_with_buffer((GtkTextBuffer*)this->buff);
	this->window = gtk_scrolled_window_new(0, 0);
//	this->window = this->txtView;
	gtk_container_add((GtkContainer*)this->window, (GtkWidget*)this->txtView);
	parent->AddChild(this);
	gtk_widget_show((GtkWidget*)this->txtView);
	this->Show();
	this->textColor = 0;
}

UI::Gtk::GtkTextView::~GtkTextView()
{
}

void UI::Gtk::GtkTextView::SetText(const UTF8Char *text)
{
	gtk_text_buffer_set_text((GtkTextBuffer*)this->buff, (const Char*)text, -1);
}

const UTF8Char *UI::Gtk::GtkTextView::GetObjectClass()
{
	return (const UTF8Char*)"TextView";
}

OSInt UI::Gtk::GtkTextView::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Gtk::GtkTextView::SetReadOnly(Bool readOnly)
{
	gtk_text_view_set_editable((::GtkTextView*)this->txtView, !readOnly);
}

Int32 UI::Gtk::GtkTextView::GetTextColor()
{
	this->textColor = textColor;
}

void UI::Gtk::GtkTextView::SetTextColor(Int32 textColor)
{
	this->textColor = textColor;
	GdkRGBA c;
	c.red = ((textColor >> 16) & 0xff) / 255.0;
	c.green = ((textColor >> 8) & 0xff) / 255.0;
	c.blue = ((textColor >> 0) & 0xff) / 255.0;
	c.alpha = ((textColor >> 24) & 0xff) / 255.0;
	gtk_widget_override_color((GtkWidget*)this->txtView, GTK_STATE_FLAG_NORMAL, &c);
}

