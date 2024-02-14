#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIClientControl.h"
#include "UI/GTK/GTKComboBox.h"
#include <gtk/gtk.h>

void GUIComboBox_EventChanged(void *window, void *userObj)
{
	UI::GUIComboBox *me = (UI::GUIComboBox*)userObj;
	me->EventSelectionChange();
}

void GUIComboBox_TextChanged(void *window, void *userObj)
{
	UI::GUIComboBox *me = (UI::GUIComboBox*)userObj;
	me->EventTextChanged();
}

void GUIComboBox_OnChild(GtkWidget *widget, gpointer data)
{
	printf("Child name = %s\r\n", gtk_widget_get_name(widget));
}

UI::GTK::GTKComboBox::GTKComboBox(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Bool allowEdit) : UI::GUIComboBox(ui, parent)
{
	this->model = 0;
	if (allowEdit)
	{
		this->hwnd = (ControlHandle*)gtk_combo_box_text_new_with_entry();
	}
	else
	{
		this->hwnd = (ControlHandle*)gtk_combo_box_text_new();
	}
	this->allowEdit = allowEdit;
	parent->AddChild(*this);
	this->Show();
	g_signal_connect(this->hwnd, "changed", G_CALLBACK(GUIComboBox_EventChanged), this);
	if (allowEdit)
	{
		GtkWidget *entry = gtk_bin_get_child((GtkBin *)this->hwnd);
		g_signal_connect(entry, "changed", G_CALLBACK(GUIComboBox_TextChanged), this);
	}
}

UI::GTK::GTKComboBox::~GTKComboBox()
{
}

void UI::GTK::GTKComboBox::SetText(Text::CStringNN text)
{
	if (this->allowEdit)
	{
		GtkWidget *entry = gtk_bin_get_child((GtkBin*)this->hwnd);
		gtk_entry_set_text((GtkEntry*)entry, (const gchar *)text.v);
	}
	else
	{
		UOSInt i = this->itemTexts.GetCount();
		NotNullPtr<Text::String> s;
		while (i-- > 0)
		{
			if (this->itemTexts.GetItem(i).SetTo(s) && s->Equals(text.v, text.leng))
			{
				this->SetSelectedIndex(i);
				return;
			}
		}
	}
}

UTF8Char *UI::GTK::GTKComboBox::GetText(UTF8Char *buff)
{
	gchar *lbl = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT((GtkWidget*)this->hwnd));
	buff = Text::StrConcat(buff, (const UTF8Char*)lbl);
	g_free(lbl);
	return buff;
}

Bool UI::GTK::GTKComboBox::GetText(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	gchar *lbl = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT((GtkWidget*)this->hwnd));
	sb->AppendSlow((const UTF8Char*)lbl);
	g_free(lbl);
	return true;
}

void UI::GTK::GTKComboBox::BeginUpdate()
{
/*	printf("wrap width = %d\r\n", gtk_combo_box_get_wrap_width(GTK_COMBO_BOX((GtkWidget*)this->hwnd)));
	gtk_combo_box_set_wrap_width(GTK_COMBO_BOX((GtkWidget*)this->hwnd), 0);

	this->clsData->model = gtk_combo_box_get_model(GTK_COMBO_BOX((GtkWidget*)this->hwnd));
	g_object_ref(this->clsData->model);
	gtk_combo_box_set_model(GTK_COMBO_BOX((GtkWidget*)this->hwnd), 0);*/
//	gtk_container_foreach(GTK_CONTAINER((GtkWidget*)this->hwnd), GUIComboBox_OnChild, this);
}

void UI::GTK::GTKComboBox::EndUpdate()
{
/*	gtk_combo_box_set_model(GTK_COMBO_BOX((GtkWidget*)this->hwnd), this->clsData->model);
	g_object_unref(this->clsData->model);
	gtk_combo_box_set_wrap_width(GTK_COMBO_BOX((GtkWidget*)this->hwnd), 0);
	this->clsData->model = 0;*/
}

UOSInt UI::GTK::GTKComboBox::AddItem(NotNullPtr<Text::String> itemText, void *itemObj)
{
	UOSInt cnt = this->itemTexts.GetCount();
	this->itemTexts.Add(itemText->Clone());
	this->items.Add(itemObj);
	if (!this->autoComplete)
	{
		gtk_combo_box_text_insert((GtkComboBoxText*)this->hwnd, -1, 0, (const Char*)itemText->v);
	}
	return cnt;
}

UOSInt UI::GTK::GTKComboBox::AddItem(Text::CStringNN itemText, void *itemObj)
{
	UOSInt cnt = this->itemTexts.GetCount();
	this->itemTexts.Add(Text::String::New(itemText));
	this->items.Add(itemObj);
	if (this->model)
	{
		GtkTreeIter iter;
		gtk_list_store_append (GTK_LIST_STORE(this->model), &iter);
		gtk_list_store_set (GTK_LIST_STORE(this->model), &iter, 0, itemText.v, -1);
	}
	else
	{
		if (!this->autoComplete)
		{
			gtk_combo_box_text_insert((GtkComboBoxText*)this->hwnd, -1, 0, (const Char*)itemText.v);
		}
	}

	return cnt;
}

UOSInt UI::GTK::GTKComboBox::InsertItem(UOSInt index, NotNullPtr<Text::String> itemText, void *itemObj)
{
	UOSInt cnt = this->itemTexts.GetCount();
	if (index >= cnt)
		index = cnt;
	this->itemTexts.Insert(index, itemText->Clone());
	this->items.Insert(index, itemObj);
	gtk_combo_box_text_insert((GtkComboBoxText*)this->hwnd, (gint)index, 0, (const Char*)itemText->v);
	return index;
}

UOSInt UI::GTK::GTKComboBox::InsertItem(UOSInt index, Text::CStringNN itemText, void *itemObj)
{
	UOSInt cnt = this->itemTexts.GetCount();
	if (index >= cnt)
		index = cnt;
	this->itemTexts.Insert(index, Text::String::New(itemText));
	this->items.Insert(index, itemObj);
	gtk_combo_box_text_insert((GtkComboBoxText*)this->hwnd, (gint)index, 0, (const Char*)itemText.v);
	return index;
}

void *UI::GTK::GTKComboBox::RemoveItem(UOSInt index)
{
	UOSInt cnt = this->itemTexts.GetCount();
	if (index >= cnt)
		return 0;
	OPTSTR_DEL(this->itemTexts.RemoveAt(index));
	void *item = this->items.RemoveAt(index);
	gtk_combo_box_text_remove((GtkComboBoxText*)this->hwnd, (gint)index);
	return item;
}

void UI::GTK::GTKComboBox::ClearItems()
{
	UOSInt i = this->itemTexts.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->itemTexts.GetItem(i));
	}
	this->itemTexts.Clear();
	this->items.Clear();
	gtk_combo_box_text_remove_all((GtkComboBoxText*)this->hwnd);
}

UOSInt UI::GTK::GTKComboBox::GetCount()
{
	return this->itemTexts.GetCount();
}

void UI::GTK::GTKComboBox::SetSelectedIndex(UOSInt index)
{
	gtk_combo_box_set_active((::GtkComboBox*)this->hwnd, (gint)index);
}

UOSInt UI::GTK::GTKComboBox::GetSelectedIndex()
{
	return (UOSInt)(OSInt)gtk_combo_box_get_active((::GtkComboBox*)this->hwnd);
}

void *UI::GTK::GTKComboBox::GetSelectedItem()
{
	return this->items.GetItem(this->GetSelectedIndex());
}

void *UI::GTK::GTKComboBox::GetItem(UOSInt index)
{
	return this->items.GetItem(index);
}

Math::Size2DDbl UI::GTK::GTKComboBox::GetSize()
{
	gint w;
	gint h;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd, &w, &h);
	return Math::Size2DDbl(w, h) * this->ddpi / this->hdpi;
	///////////////////////////////
}

void UI::GTK::GTKComboBox::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	this->SetAreaP(Double2Int32(left * this->hdpi / this->ddpi),
		Double2Int32(top * this->hdpi / this->ddpi),
		Double2Int32(right * this->hdpi / this->ddpi),
		Double2Int32(bottom * this->hdpi / this->ddpi), updateScn);
}

OSInt UI::GTK::GTKComboBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GTK::GTKComboBox::UpdatePos(Bool redraw)
{
	/////////////////////////	
}

void UI::GTK::GTKComboBox::SetTextSelection(UOSInt startPos, UOSInt endPos)
{
	if (this->allowEdit)
	{
		GtkWidget *entry = gtk_bin_get_child((GtkBin*)this->hwnd);
		gtk_editable_select_region((GtkEditable*)entry, (gint)startPos, (gint)endPos);
	}
}
