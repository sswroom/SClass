#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayList.h"
#include "UI/GUIRadioButton.h"
#include <gtk/gtk.h>

void GUIRadioButton_SelChange(GtkRadioButton *btn, gpointer data)
{
	UI::GUIRadioButton *me = (UI::GUIRadioButton *)data;
	Bool sel = gtk_toggle_button_get_active((GtkToggleButton*)me->GetHandle());
	me->ChangeSelected(sel);
}

void UI::GUIRadioButton::ChangeSelected(Bool selVal)
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
	UOSInt i = this->selectedChangeHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selectedChangeHdlrs->GetItem(i)(this->selectedChangeObjs->GetItem(i), this->selected);
	}
}

UI::GUIRadioButton::GUIRadioButton(UI::GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *initText, Bool selected) : UI::GUIControl(ui, parent)
{
	this->selected = selected;
	NEW_CLASS(this->selectedChangeHdlrs, Data::ArrayList<SelectedChangeHandler>());
	NEW_CLASS(this->selectedChangeObjs, Data::ArrayList<void *>());

	GUIRadioButton *radioBtn = 0;
	UOSInt i = 0;
	UOSInt j = this->parent->GetChildCount();
	while (i < j)
	{
		UI::GUIControl *ctrl = this->parent->GetChild(i);
		if (ctrl != this)
		{
			if (ctrl->GetObjectClass().Equals(UTF8STRC("RadioButton")))
			{
				radioBtn = (GUIRadioButton*)ctrl;
			}
		}
		i++;
	}
	
	if (radioBtn)
	{
		this->hwnd = (ControlHandle*)gtk_radio_button_new_with_label_from_widget((GtkRadioButton*)radioBtn->GetHandle(), (const Char*)initText);
	}
	else
	{
		this->hwnd = (ControlHandle*)gtk_radio_button_new_with_label(0, (const Char*)initText);
	}
	parent->AddChild(this);
	this->Show();

	g_signal_connect(this->hwnd, "toggled", G_CALLBACK(GUIRadioButton_SelChange), this);
	if (selected)
		Select();
}

UI::GUIRadioButton::~GUIRadioButton()
{
	DEL_CLASS(this->selectedChangeHdlrs);
	DEL_CLASS(this->selectedChangeObjs);
}

Text::CString UI::GUIRadioButton::GetObjectClass()
{
	return CSTR("RadioButton");
}

OSInt UI::GUIRadioButton::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

Bool UI::GUIRadioButton::IsSelected()
{
	return this->selected;
}

void UI::GUIRadioButton::Select()
{
	gtk_toggle_button_set_active((GtkToggleButton*)this->hwnd, true);
}

void UI::GUIRadioButton::HandleSelectedChange(SelectedChangeHandler hdlr, void *userObj)
{
	this->selectedChangeHdlrs->Add(hdlr);
	this->selectedChangeObjs->Add(userObj);
}

