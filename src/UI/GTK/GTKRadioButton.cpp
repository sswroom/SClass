#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayList.h"
#include "UI/GTK/GTKRadioButton.h"

void UI::GTK::GTKRadioButton::SignalToggled(GtkRadioButton *btn, gpointer data)
{
	UI::GTK::GTKRadioButton *me = (UI::GTK::GTKRadioButton *)data;
	Bool sel = gtk_toggle_button_get_active((GtkToggleButton*)me->GetHandle());
	me->ChangeSelected(sel);
}

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
	UOSInt i = 0;
	UOSInt j = parent->GetChildCount();
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
	
	if (radioBtn)
	{
		this->hwnd = (ControlHandle*)gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)radioBtn->GetHandle(), (const Char*)initText.v.Ptr());
	}
	else
	{
		this->hwnd = (ControlHandle*)gtk_radio_button_new_with_label(0, (const Char*)initText.v.Ptr());
	}
	parent->AddChild(*this);
	this->Show();

	g_signal_connect(this->hwnd, "toggled", G_CALLBACK(SignalToggled), this);
	if (selected)
		Select();
}

UI::GTK::GTKRadioButton::~GTKRadioButton()
{
}

OSInt UI::GTK::GTKRadioButton::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::GTK::GTKRadioButton::IsSelected()
{
	return this->selected;
}

void UI::GTK::GTKRadioButton::Select()
{
	gtk_toggle_button_set_active((GtkToggleButton*)this->hwnd, true);
}
