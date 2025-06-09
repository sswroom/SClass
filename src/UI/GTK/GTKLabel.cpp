#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKLabel.h"
#include <gtk/gtk.h>

#define GDK_VERSION_AFTER(major, minor) (GDK_MAJOR_VERSION > major || (GDK_MAJOR_VERSION == major && GDK_MINOR_VERSION >= minor))

UI::GTK::GTKLabel::GTKLabel(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN label) : UI::GUILabel(ui, parent)
{
	this->hwnd = (ControlHandle*)gtk_label_new((const Char*)label.v.Ptr());
	parent->AddChild(*this);
#if GDK_VERSION_AFTER(3, 16)
	gtk_label_set_xalign(GTK_LABEL((GtkWidget*)this->hwnd.OrNull()), 0.0);
#elif GDK_VERSION_AFTER(3, 14)
	gtk_widget_set_halign((GtkWidget*)this->hwnd.OrNull(), GTK_ALIGN_START);
#else
	gtk_misc_set_alignment(GTK_MISC((GtkWidget*)this->hwnd.OrNull()), 0.0, 0.0);
#endif
	this->Show();
	this->hasTextColor = false;
	this->textColor = 0;
}

UI::GTK::GTKLabel::~GTKLabel()
{
}

void UI::GTK::GTKLabel::SetText(Text::CStringNN text)
{
	gtk_label_set_text((GtkLabel*)this->hwnd.OrNull(), (const Char*)text.v.Ptr());
}

OSInt UI::GTK::GTKLabel::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::GTK::GTKLabel::HasTextColor()
{
	return this->hasTextColor;
}

UInt32 UI::GTK::GTKLabel::GetTextColor()
{
	return this->textColor;
}

void UI::GTK::GTKLabel::SetTextColor(UInt32 textColor)
{
	this->textColor = textColor;
	this->hasTextColor = true;
#if GDK_VERSION_AFTER(3, 16)
	Text::CSSBuilder builder(Text::CSSBuilder::PM_SPACE);
	builder.NewStyle(CSTR_NULL, CSTR_NULL);
	builder.AddColorRGBA(textColor);
	GtkStyleContext *style = gtk_widget_get_style_context((GtkWidget*)this->hwnd.OrNull());
	GtkCssProvider *styleProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(styleProvider, (const gchar*)builder.ToString().Ptr(), -1, 0);
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

