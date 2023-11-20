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

UI::GUICheckBox::GUICheckBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked) : UI::GUIControl(ui, parent)
{
	this->checked = false;

	this->hwnd = (ControlHandle*)gtk_check_button_new_with_label((const Char*)initText.v);
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
}

void UI::GUICheckBox::SetText(Text::CStringNN text)
{
	GList *children = gtk_container_get_children((GtkContainer*)this->hwnd);
	if (children)
	{
		gtk_label_set_text((GtkLabel*)children->data, (const Char*)text.v);
	}
	g_list_free(children);
}

Text::CStringNN UI::GUICheckBox::GetObjectClass() const
{
	return CSTR("CheckBox");
}

OSInt UI::GUICheckBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUICheckBox::EventCheckedChange(Bool newState)
{
	UOSInt i = this->checkedChangeHdlrs.GetCount();
	while (i-- > 0)
	{
		this->checkedChangeHdlrs.GetItem(i)(this->checkedChangeObjs.GetItem(i), newState);
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
	this->checkedChangeHdlrs.Add(hdlr);
	this->checkedChangeObjs.Add(obj);
}

