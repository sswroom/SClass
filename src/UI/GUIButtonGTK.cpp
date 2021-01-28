#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIButton.h"
#include "UI/GUIClientControl.h"
#include <gtk/gtk.h>

void GUIButton_EventClicked(void *window, void *userObj)
{
	UI::GUIButton *me = (UI::GUIButton*)userObj;
	me->EventButtonClick();
}

void GUIButton_OnPress(void *window, void *userObj)
{
	UI::GUIButton *me = (UI::GUIButton*)userObj;
	me->EventButtonDown();
}

void GUIButton_OnRelease(void *window, void *userObj)
{
	UI::GUIButton *me = (UI::GUIButton*)userObj;
	me->EventButtonUp();
}

gboolean GUIButton_OnFocus(void *window, GtkDirectionType direction, void *userObj)
{
	UI::GUIButton *me = (UI::GUIButton*)userObj;
	me->OnFocus();
	return FALSE;
}

gboolean GUIButton_OnFocusLost(void *window, void *userObj)
{
	UI::GUIButton *me = (UI::GUIButton*)userObj;
	me->OnFocusLost();
	return FALSE;
}

UI::GUIButton::GUIButton(UI::GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *label) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->btnClkHandlers, Data::ArrayList<UIEvent>());
	NEW_CLASS(this->btnClkHandlersObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->btnUpDownHandlers, Data::ArrayList<UpDownEvent>());
	NEW_CLASS(this->btnUpDownHandlersObjs, Data::ArrayList<void *>());

	this->hwnd = gtk_button_new();
	this->SetText(label);
//	gtk_container_set_border_width((GtkContainer*)this->hwnd, 0);
//	GtkStyleContext *context;
//	context = gtk_widget_get_style_context(GTK_WIDGET(this->hwnd));
//	gtk_style_context_set_paGTK_STATE_FLAG_NORMAL
	g_signal_connect((GtkButton*)this->hwnd, "clicked", G_CALLBACK(GUIButton_EventClicked), this);
	g_signal_connect((GtkButton*)this->hwnd, "focus", G_CALLBACK(GUIButton_OnFocus), this);
	g_signal_connect((GtkButton*)this->hwnd, "grab-broken-event", G_CALLBACK(GUIButton_OnFocusLost), this);
	g_signal_connect((GtkButton*)this->hwnd, "pressed", G_CALLBACK(GUIButton_OnPress), this);
	g_signal_connect((GtkButton*)this->hwnd, "released", G_CALLBACK(GUIButton_OnRelease), this);
	parent->AddChild(this);
	this->Show();
}

UI::GUIButton::~GUIButton()
{
	DEL_CLASS(this->btnClkHandlers);
	DEL_CLASS(this->btnClkHandlersObjs);
	DEL_CLASS(this->btnUpDownHandlers);
	DEL_CLASS(this->btnUpDownHandlersObjs);
}

void UI::GUIButton::SetText(const UTF8Char *text)
{
	UTF8Char *cptr;
	UTF8Char c;
	Bool hasUL = false;
	const UTF8Char *lbl = Text::StrCopyNew(text);
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
		gtk_button_set_use_underline((GtkButton*)this->hwnd, TRUE);
	}
	gtk_button_set_label((GtkButton*)this->hwnd, (const Char*)lbl);
	Text::StrDelNew(lbl);
}

void UI::GUIButton::SetFont(const UTF8Char *name, Double fontHeight, Bool isBold)
{
	PangoFontDescription *font = pango_font_description_new();
	if (name)
	{
		pango_font_description_set_family(font, (const Char*)name);
	}
	pango_font_description_set_absolute_size(font, fontHeight * PANGO_SCALE);
	if (isBold)
		pango_font_description_set_weight(font, PANGO_WEIGHT_BOLD);
	gtk_widget_override_font(gtk_bin_get_child((GtkBin*)this->hwnd), font); 	
	pango_font_description_free(font);
}

const UTF8Char *UI::GUIButton::GetObjectClass()
{
	return (const UTF8Char*)"Button";
}

OSInt UI::GUIButton::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

void UI::GUIButton::OnFocus()
{
}

void UI::GUIButton::OnFocusLost()
{
}

void UI::GUIButton::EventButtonClick()
{
	OSInt i;
	i = this->btnClkHandlers->GetCount();
	while (i-- > 0)
	{
		this->btnClkHandlers->GetItem(i)(this->btnClkHandlersObjs->GetItem(i));
	}
}

void UI::GUIButton::EventButtonDown()
{
	OSInt i;
	i = this->btnUpDownHandlers->GetCount();
	while (i-- > 0)
	{
		this->btnUpDownHandlers->GetItem(i)(this->btnUpDownHandlersObjs->GetItem(i), true);
	}
}

void UI::GUIButton::EventButtonUp()
{
	OSInt i;
	i = this->btnUpDownHandlers->GetCount();
	while (i-- > 0)
	{
		this->btnUpDownHandlers->GetItem(i)(this->btnUpDownHandlersObjs->GetItem(i), false);
	}
}

void UI::GUIButton::HandleButtonClick(UIEvent handler, void *userObj)
{
	this->btnClkHandlers->Add(handler);
	this->btnClkHandlersObjs->Add(userObj);
}

void UI::GUIButton::HandleButtonUpDown(UpDownEvent handler, void *userObj)
{
	this->btnUpDownHandlers->Add(handler);
	this->btnUpDownHandlersObjs->Add(userObj);
}
