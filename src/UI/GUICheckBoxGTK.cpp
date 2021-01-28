#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIClientControl.h"
#include <gtk/gtk.h>

void GUICheckBox_CheckedChange(GtkToggleButton *btn, gpointer data)
{
	UI::GUICheckBox *me = (UI::GUICheckBox*)data;
	me->EventCheckedChange(me->IsChecked());
}

UI::GUICheckBox::GUICheckBox(GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *initText, Bool checked) : UI::GUIControl(ui, parent)
{
	this->checked = false;
	NEW_CLASS(this->checkedChangeHdlrs, Data::ArrayList<CheckedChangeHandler>());
	NEW_CLASS(this->checkedChangeObjs, Data::ArrayList<void *>());

	this->hwnd = gtk_check_button_new_with_label((const Char*)initText);
	parent->AddChild(this);
	if (checked)
	{
		this->SetChecked(checked);
	}
	g_signal_connect((GtkWidget*)this->hwnd, "toggled", G_CALLBACK(GUICheckBox_CheckedChange), this);
	this->Show();
}

UI::GUICheckBox::~GUICheckBox()
{
	DEL_CLASS(this->checkedChangeHdlrs);
	DEL_CLASS(this->checkedChangeObjs);
}

const UTF8Char *UI::GUICheckBox::GetObjectClass()
{
	return (const UTF8Char*)"CheckBox";
}

OSInt UI::GUICheckBox::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

void UI::GUICheckBox::EventCheckedChange(Bool newState)
{
	OSInt i = this->checkedChangeHdlrs->GetCount();
	while (i-- > 0)
	{
		this->checkedChangeHdlrs->GetItem(i)(this->checkedChangeObjs->GetItem(i), newState);
	}
}

Bool UI::GUICheckBox::IsChecked()
{
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON((GtkWidget*)this->hwnd));
}

void UI::GUICheckBox::SetChecked(Bool checked)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((GtkWidget*)this->hwnd), checked?TRUE:FALSE);
}

void UI::GUICheckBox::HandleCheckedChange(CheckedChangeHandler hdlr, void *obj)
{
	this->checkedChangeHdlrs->Add(hdlr);
	this->checkedChangeObjs->Add(obj);
}

