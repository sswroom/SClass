#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "UI/GUILabel.h"
#include "UI/GUIClientControl.h"
#include <gtk/gtk.h>

#define GDK_VERSION_AFTER(major, minor) (GDK_MAJOR_VERSION > major || (GDK_MAJOR_VERSION == major && GDK_MINOR_VERSION >= minor))

UI::GUILabel::GUILabel(UI::GUICore *ui, UI::GUIClientControl *parent, Text::CString label) : UI::GUIControl(ui, parent)
{
	this->hwnd = (ControlHandle*)gtk_label_new((const Char*)label.v);
	parent->AddChild(this);
#if GDK_VERSION_AFTER(3, 16)
	gtk_label_set_xalign(GTK_LABEL((GtkWidget*)this->hwnd), 0.0);
#elif GDK_VERSION_AFTER(3, 14)
	gtk_widget_set_halign((GtkWidget*)this->hwnd, GTK_ALIGN_START);
#else
	gtk_misc_set_alignment(GTK_MISC((GtkWidget*)this->hwnd), 0.0, 0.0);
#endif
	this->Show();
	this->hasTextColor = false;
	this->textColor = 0;
}

UI::GUILabel::~GUILabel()
{
}

void UI::GUILabel::SetText(Text::CString text)
{
	gtk_label_set_text((GtkLabel*)this->hwnd, (const Char*)text.v);
}

Text::CString UI::GUILabel::GetObjectClass()
{
	return CSTR("Label");
}

OSInt UI::GUILabel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::GUILabel::HasTextColor()
{
	return this->hasTextColor;
}

UInt32 UI::GUILabel::GetTextColor()
{
	return this->textColor;
}

void UI::GUILabel::SetTextColor(UInt32 textColor)
{
	this->textColor = textColor;
	this->hasTextColor = true;
#if GDK_VERSION_AFTER(3, 16)
	Text::CSSBuilder builder(Text::CSSBuilder::PM_SPACE);
	builder.NewStyle(CSTR_NULL, CSTR_NULL);
	builder.AddColorRGBA(textColor);
	GtkStyleContext *style = gtk_widget_get_style_context((GtkWidget*)this->hwnd);
	GtkCssProvider *styleProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(styleProvider, (const gchar*)builder.ToString(), -1, 0);
	gtk_style_context_add_provider(style, (GtkStyleProvider*)styleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
#else
	GdkRGBA c;
	c.red = ((textColor >> 16) & 0xff) / 255.0;
	c.green = ((textColor >> 8) & 0xff) / 255.0;
	c.blue = ((textColor >> 0) & 0xff) / 255.0;
	c.alpha = ((textColor >> 24) & 0xff) / 255.0;
	gtk_widget_override_color((GtkWidget*)this->hwnd, GTK_STATE_FLAG_NORMAL, &c);
#endif
}

