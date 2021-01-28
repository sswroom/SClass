#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUILabel.h"
#include "UI/GUIClientControl.h"
#include <gtk/gtk.h>

UI::GUILabel::GUILabel(UI::GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *label) : UI::GUIControl(ui, parent)
{
	this->hwnd = gtk_label_new((const Char*)label);
	parent->AddChild(this);
#if GTK_MAJOR_VERSION == 3
#if GTK_MINOR_VERSION >= 16
	gtk_label_set_xalign(GTK_LABEL((GtkWidget*)this->hwnd), 0.0);
#elif GTK_MINOR_VERSION < 14
	gtk_misc_set_alignment(GTK_MISC((GtkWidget*)this->hwnd), 0.0, 0.0);
#else
	gtk_widget_set_halign((GtkWidget*)this->hwnd, GTK_ALIGN_START);
#endif
#endif
	this->Show();
	this->hasTextColor = false;
	this->textColor = 0;
}

UI::GUILabel::~GUILabel()
{
}

void UI::GUILabel::SetText(const UTF8Char *text)
{
	gtk_label_set_text((GtkLabel*)this->hwnd, (const Char*)text);
}

const UTF8Char *UI::GUILabel::GetObjectClass()
{
	return (const UTF8Char*)"Label";
}

OSInt UI::GUILabel::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

Bool UI::GUILabel::HasTextColor()
{
	return this->hasTextColor;
}

Int32 UI::GUILabel::GetTextColor()
{
	return this->textColor;
}

void UI::GUILabel::SetTextColor(Int32 textColor)
{
	this->textColor = textColor;
	this->hasTextColor = true;
	GdkRGBA c;
	c.red = ((textColor >> 16) & 0xff) / 255.0;
	c.green = ((textColor >> 8) & 0xff) / 255.0;
	c.blue = ((textColor >> 0) & 0xff) / 255.0;
	c.alpha = ((textColor >> 24) & 0xff) / 255.0;
	gtk_widget_override_color((GtkWidget*)this->hwnd, GTK_STATE_FLAG_NORMAL, &c);
}

