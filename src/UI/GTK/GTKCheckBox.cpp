#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKCheckBox.h"

void UI::GTK::GTKCheckBox::SignalToggled(GtkToggleButton *btn, gpointer data)
{
	UI::GTK::GTKCheckBox *me = (UI::GTK::GTKCheckBox*)data;
	me->EventCheckedChange(me->checked);
}

UI::GTK::GTKCheckBox::GTKCheckBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked) : UI::GUICheckBox(ui, parent)
{
	this->hwnd = (ControlHandle*)gtk_check_button_new_with_label((const Char*)initText.v);
	parent->AddChild(*this);
	if (checked)
	{
		this->SetChecked(checked);
	}
	g_signal_connect((GtkWidget*)this->hwnd, "toggled", G_CALLBACK(SignalToggled), this);
	this->Show();
}

UI::GTK::GTKCheckBox::~GTKCheckBox()
{
}

void UI::GTK::GTKCheckBox::SetText(Text::CStringNN text)
{
	GList *children = gtk_container_get_children((GtkContainer*)this->hwnd);
	if (children)
	{
		gtk_label_set_text((GtkLabel*)children->data, (const Char*)text.v);
	}
	g_list_free(children);
}

OSInt UI::GTK::GTKCheckBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::GTK::GTKCheckBox::IsChecked()
{
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON((GtkWidget*)this->hwnd));
}

void UI::GTK::GTKCheckBox::SetChecked(Bool checked)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((GtkWidget*)this->hwnd), checked?TRUE:FALSE);
}


