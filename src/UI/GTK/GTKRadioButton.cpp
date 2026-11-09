#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GTK/GTKRadioButton.h"

#if GTK_CHECK_VERSION(4, 0, 0)
void UI::GTK::GTKRadioButton::SignalToggled(GtkCheckButton *btn, gpointer data)
{
	UI::GTK::GTKRadioButton *me = (UI::GTK::GTKRadioButton *)data;
	Bool sel = gtk_check_button_get_active((GtkCheckButton*)me->GetHandle().OrNull());
	me->ChangeSelected(sel);
}
#else
void UI::GTK::GTKRadioButton::SignalToggled(GtkRadioButton *btn, gpointer data)
{
	UI::GTK::GTKRadioButton *me = (UI::GTK::GTKRadioButton *)data;
	Bool sel = gtk_toggle_button_get_active((GtkToggleButton*)me->GetHandle().OrNull());
	me->ChangeSelected(sel);
}
#endif

void UI::GTK::GTKRadioButton::ChangeSelected(Bool selVal)
{
	if (this->selected == selVal)
		return;
	if (selVal)
	{
		this->selected = true;
	}
	else
	{
		this->selected = false;
	}
	this->EventSelectedChange(selVal);
}

UI::GTK::GTKRadioButton::GTKRadioButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool selected) : UI::GUIRadioButton(ui, parent)
{
	this->selected = selected;

	GUIRadioButton *radioBtn = 0;
	UIntOS i = 0;
	UIntOS j = parent->GetChildCount();
	while (i < j)
	{
		NN<UI::GUIControl> ctrl;
		if (parent->GetChild(i).SetTo(ctrl) && ctrl.Ptr() != this)
		{
			if (ctrl->GetObjectClass().Equals(UTF8STRC("RadioButton")))
			{
				radioBtn = (GUIRadioButton*)ctrl.Ptr();
			}
		}
		i++;
	}
	
#if GTK_CHECK_VERSION(4, 0, 0)
	this->hwnd = (ControlHandle*)gtk_check_button_new_with_label((const Char*)initText.v.Ptr());
	if (radioBtn)
	{
		gtk_check_button_set_group(GTK_CHECK_BUTTON(this->hwnd.OrNull()), GTK_CHECK_BUTTON(radioBtn->GetHandle().OrNull()));
	}
#else
	if (radioBtn)
	{
		this->hwnd = (ControlHandle*)gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)radioBtn->GetHandle().OrNull(), (const Char*)initText.v.Ptr());
	}
	else
	{
		this->hwnd = (ControlHandle*)gtk_radio_button_new_with_label(0, (const Char*)initText.v.Ptr());
	}
#endif
	parent->AddChild(*this);
	this->Show();

	g_signal_connect(this->hwnd.OrNull(), "toggled", G_CALLBACK(SignalToggled), this);
	if (selected)
		Select();
}

UI::GTK::GTKRadioButton::~GTKRadioButton()
{
}

IntOS UI::GTK::GTKRadioButton::OnNotify(UInt32 code, IntOS lParam)
{
	return 0;
}

Bool UI::GTK::GTKRadioButton::IsSelected()
{
	return this->selected;
}

void UI::GTK::GTKRadioButton::Select()
{
#if GTK_CHECK_VERSION(4, 0, 0)
	gtk_check_button_set_active((GtkCheckButton*)this->hwnd.OrNull(), true);
#else
	gtk_toggle_button_set_active((GtkToggleButton*)this->hwnd.OrNull(), true);
#endif
}
