#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/CSSBuilder.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKButton.h"

#define GDK_VERSION_AFTER(major, minor) (GDK_MAJOR_VERSION > major || (GDK_MAJOR_VERSION == major && GDK_MINOR_VERSION >= minor))

void UI::GTK::GTKButton::SignalClicked(void *window, void *userObj)
{
	UI::GTK::GTKButton *me = (UI::GTK::GTKButton*)userObj;
	me->EventButtonClick();
}

void UI::GTK::GTKButton::SignalPress(void *window, void *userObj)
{
	UI::GTK::GTKButton *me = (UI::GTK::GTKButton*)userObj;
	me->EventButtonDown();
}

void UI::GTK::GTKButton::SignalRelease(void *window, void *userObj)
{
	UI::GTK::GTKButton *me = (UI::GTK::GTKButton*)userObj;
	me->EventButtonUp();
}

gboolean UI::GTK::GTKButton::SignalFocus(void *window, GtkDirectionType direction, void *userObj)
{
	UI::GTK::GTKButton *me = (UI::GTK::GTKButton*)userObj;
	me->EventFocus();
	return FALSE;
}

gboolean UI::GTK::GTKButton::SignalFocusLost(void *window, void *userObj)
{
	UI::GTK::GTKButton *me = (UI::GTK::GTKButton*)userObj;
	me->EventFocusLost();
	return FALSE;
}

UI::GTK::GTKButton::GTKButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN label) : UI::GUIButton(ui, parent)
{
	this->hwnd = (ControlHandle*)gtk_button_new();
	this->SetText(label);
//	gtk_container_set_border_width((GtkContainer*)this->hwnd, 0);
//	GtkStyleContext *context;
//	context = gtk_widget_get_style_context(GTK_WIDGET(this->hwnd));
//	gtk_style_context_set_paGTK_STATE_FLAG_NORMAL
	g_signal_connect((GtkButton*)this->hwnd.OrNull(), "clicked", G_CALLBACK(SignalClicked), this);
	g_signal_connect((GtkButton*)this->hwnd.OrNull(), "focus", G_CALLBACK(SignalFocus), this);
	g_signal_connect((GtkButton*)this->hwnd.OrNull(), "grab-broken-event", G_CALLBACK(SignalFocusLost), this);
	g_signal_connect((GtkButton*)this->hwnd.OrNull(), "pressed", G_CALLBACK(SignalPress), this);
	g_signal_connect((GtkButton*)this->hwnd.OrNull(), "released", G_CALLBACK(SignalRelease), this);
	parent->AddChild(*this);
	this->Show();
}

UI::GTK::GTKButton::~GTKButton()
{
}

void UI::GTK::GTKButton::SetText(Text::CStringNN text)
{
	UTF8Char *cptr;
	UTF8Char c;
	Bool hasUL = false;
	const UTF8Char *lbl = Text::StrCopyNewC(text.v, text.leng).Ptr();
	cptr = (UTF8Char*)lbl;
	while ((c = *cptr++) != 0)
	{
		if (c == '&')
		{
			cptr[-1] = '_';
			hasUL = true;
		}
	}
	if (hasUL)
	{
		gtk_button_set_use_underline((GtkButton*)this->hwnd.OrNull(), TRUE);
	}
	gtk_button_set_label((GtkButton*)this->hwnd.OrNull(), (const Char*)lbl);
	Text::StrDelNew(lbl);
}

void UI::GTK::GTKButton::SetFont(UnsafeArrayOpt<const UTF8Char> name, UOSInt nameLen, Double fontHeightPt, Bool isBold)
{
	GtkWidget *widget = gtk_bin_get_child((GtkBin*)this->hwnd.OrNull());
#if GDK_VERSION_AFTER(3, 16)
	Text::CSSBuilder builder(Text::CSSBuilder::PM_SPACE);
	builder.NewStyle(CSTR("label"), CSTR_NULL);
	UnsafeArray<const UTF8Char> nnname;
	if (name.SetTo(nnname)) builder.AddFontFamily(nnname);
	if (fontHeightPt != 0) builder.AddFontSize(fontHeightPt * this->hdpi / this->ddpi, Math::Unit::Distance::DU_PIXEL);
	if (isBold) builder.AddFontWeight(Text::CSSBuilder::FONT_WEIGHT_BOLD);

	GtkStyleContext *style = gtk_widget_get_style_context(widget);
	GtkCssProvider *styleProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(styleProvider, (const gchar*)builder.ToString().Ptr(), -1, 0);
	gtk_style_context_add_provider(style, (GtkStyleProvider*)styleProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_widget_reset_style(widget);
#else
	PangoFontDescription *font = pango_font_description_new();
	UnsafeArray<const UTF8Char> nnname;
	if (name.SetTo(nnname))
	{
		pango_font_description_set_family(font, (const Char*)nnname.Ptr());
	}
	pango_font_description_set_absolute_size(font, fontHeightPt * this->hdpi / this->ddpi * PANGO_SCALE / 0.75);
	if (isBold)
		pango_font_description_set_weight(font, PANGO_WEIGHT_BOLD);
	gtk_widget_override_font(widget, font); 	
	pango_font_description_free(font);
#endif
}

OSInt UI::GTK::GTKButton::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
