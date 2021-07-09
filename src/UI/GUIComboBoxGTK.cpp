#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIComboBox.h"
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

UI::GUIComboBox::GUIComboBox(UI::GUICore *ui, UI::GUIClientControl *parent, Bool allowEdit) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->selChgHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->selChgObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->items, Data::ArrayList<void*>());
	NEW_CLASS(this->itemTexts, Data::ArrayList<const UTF8Char *>());
	if (allowEdit)
	{
		this->hwnd = gtk_combo_box_text_new_with_entry();
	}
	else
	{
		this->hwnd = gtk_combo_box_text_new();
	}
	this->allowEdit = allowEdit;
	this->autoComplete = false;
	this->nonUIEvent = false;
	parent->AddChild(this);
	this->Show();
	g_signal_connect(this->hwnd, "changed", G_CALLBACK(GUIComboBox_EventChanged), this);
	if (allowEdit)
	{
		GtkWidget *entry = gtk_bin_get_child((GtkBin *)this->hwnd);
		g_signal_connect(entry, "changed", G_CALLBACK(GUIComboBox_TextChanged), this);
	}
}

UI::GUIComboBox::~GUIComboBox()
{
	OSInt i = this->itemTexts->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->itemTexts->GetItem(i));
	}
	DEL_CLASS(this->itemTexts);
	DEL_CLASS(this->items);
	DEL_CLASS(this->selChgHdlrs);
	DEL_CLASS(this->selChgObjs);
}

void UI::GUIComboBox::EventSelectionChange()
{
	OSInt i = this->selChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs->GetItem(i)(this->selChgObjs->GetItem(i));
	}
}

void UI::GUIComboBox::EventTextChanged()
{
	if (this->autoComplete || !this->nonUIEvent)
	{
		Text::StringBuilderUTF8 sb;
		this->GetText(&sb);
		if (sb.GetLength() > this->lastTextLeng)
		{
			this->nonUIEvent = true;
			UOSInt i = 0;
			UOSInt j = this->itemTexts->GetCount();
			while (i < j)
			{
				if (Text::StrStartsWith(this->itemTexts->GetItem(i), sb.ToString()))
				{
					this->SetSelectedIndex(i);
					this->SetTextSelection(sb.GetLength(), Text::StrCharCnt(this->itemTexts->GetItem(i)));
					break;
				}
				i++;
			}
			this->nonUIEvent = false;
		}
		this->lastTextLeng = sb.GetLength();
	}
}

void UI::GUIComboBox::SetText(const UTF8Char *text)
{
	if (this->allowEdit)
	{
		GtkWidget *entry = gtk_bin_get_child((GtkBin*)this->hwnd);
		gtk_entry_set_text((GtkEntry*)entry, (const gchar *)text);
	}
	else
	{
		OSInt i = this->itemTexts->GetCount();
		while (i-- > 0)
		{
			if (Text::StrEquals(text, this->itemTexts->GetItem(i)))
			{
				this->SetSelectedIndex(i);
				return;
			}
		}
	}
}

UTF8Char *UI::GUIComboBox::GetText(UTF8Char *buff)
{
	gchar *lbl = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT((GtkWidget*)this->hwnd));
	return Text::StrConcat(buff, (const UTF8Char*)lbl);
}

Bool UI::GUIComboBox::GetText(Text::StringBuilderUTF *sb)
{
	gchar *lbl = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT((GtkWidget*)this->hwnd));
	sb->Append((const UTF8Char*)lbl);
	return true;
}

UOSInt UI::GUIComboBox::AddItem(const UTF8Char *itemText, void *itemObj)
{
	UOSInt cnt = this->itemTexts->GetCount();
	this->itemTexts->Add(Text::StrCopyNew(itemText));
	this->items->Add(itemObj);
	gtk_combo_box_text_insert((GtkComboBoxText*)this->hwnd, -1, 0, (const Char*)itemText);
	return cnt;
}


UOSInt UI::GUIComboBox::InsertItem(UOSInt index, const UTF8Char *itemText, void *itemObj)
{
	UOSInt cnt = this->itemTexts->GetCount();
	if (index >= cnt)
		index = cnt;
	this->itemTexts->Insert(index, Text::StrCopyNew(itemText));
	this->items->Insert(index, itemObj);
	gtk_combo_box_text_insert((GtkComboBoxText*)this->hwnd, index, 0, (const Char*)itemText);
	return index;
}

void *UI::GUIComboBox::RemoveItem(UOSInt index)
{
	UOSInt cnt = this->itemTexts->GetCount();
	if (index >= cnt)
		return 0;
	const UTF8Char *txt = this->itemTexts->RemoveAt(index);
	void *item = this->items->RemoveAt(index);
	Text::StrDelNew(txt);
	gtk_combo_box_text_remove((GtkComboBoxText*)this->hwnd, index);
	return item;
}

void UI::GUIComboBox::ClearItems()
{
	UOSInt i = this->itemTexts->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->itemTexts->GetItem(i));
	}
	this->itemTexts->Clear();
	this->items->Clear();
	gtk_combo_box_text_remove_all((GtkComboBoxText*)this->hwnd);
}

UOSInt UI::GUIComboBox::GetCount()
{
	return this->itemTexts->GetCount();
}

void UI::GUIComboBox::SetSelectedIndex(UOSInt index)
{
	gtk_combo_box_set_active((::GtkComboBox*)this->hwnd, index);
}

OSInt UI::GUIComboBox::GetSelectedIndex()
{
	return gtk_combo_box_get_active((::GtkComboBox*)this->hwnd);
}

UTF8Char *UI::GUIComboBox::GetSelectedItemText(UTF8Char *sbuff)
{
	OSInt i = this->GetSelectedIndex();
	if (i == -1)
		return 0;
	return Text::StrConcat(sbuff, this->itemTexts->GetItem(i));
}

void *UI::GUIComboBox::GetSelectedItem()
{
	return this->items->GetItem(this->GetSelectedIndex());
}

UTF8Char *UI::GUIComboBox::GetItemText(UTF8Char *buff, UOSInt index)
{
	const UTF8Char *txt = this->itemTexts->GetItem(index);
	if (txt == 0)
		return 0;
	return Text::StrConcat(buff, txt);
}

void *UI::GUIComboBox::GetItem(UOSInt index)
{
	return this->items->GetItem(index);
}

void UI::GUIComboBox::GetSize(Double *width, Double *height)
{
	gint w;
	gint h;
	gtk_widget_get_size_request((GtkWidget*)this->hwnd, &w, &h);
	*width = w * this->ddpi / this->hdpi;
	*height = h * this->ddpi / this->hdpi;
	///////////////////////////////
}

void UI::GUIComboBox::SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn)
{
	this->SetAreaP(Math::Double2Int32(left * this->hdpi / this->ddpi),
		Math::Double2Int32(top * this->hdpi / this->ddpi),
		Math::Double2Int32(right * this->hdpi / this->ddpi),
		Math::Double2Int32(bottom * this->hdpi / this->ddpi), updateScn);
}

const UTF8Char *UI::GUIComboBox::GetObjectClass()
{
	return (const UTF8Char*)"ComboBox";
}

OSInt UI::GUIComboBox::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

void UI::GUIComboBox::HandleSelectionChange(UI::UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs->Add(hdlr);
	this->selChgObjs->Add(userObj);
}

void UI::GUIComboBox::UpdatePos(Bool redraw)
{
	/////////////////////////	
}

void UI::GUIComboBox::SetAutoComplete(Bool autoComplete)
{
	if (!this->autoComplete && autoComplete)
	{
		Text::StringBuilderUTF8 sb;
		this->GetText(&sb);
		this->lastTextLeng = sb.GetLength();
	}
	this->autoComplete = autoComplete;
}

void UI::GUIComboBox::SetTextSelection(OSInt startPos, OSInt endPos)
{
	if (this->allowEdit)
	{
		GtkWidget *entry = gtk_bin_get_child((GtkBin*)this->hwnd);
		gtk_editable_select_region((GtkEditable*)entry, 0, 1);
	}
}
