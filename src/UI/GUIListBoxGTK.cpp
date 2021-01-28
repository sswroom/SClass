#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUIListBox.h"
#include <gtk/gtk.h>

typedef struct
{
	GtkWidget *listbox;
	Bool isShown;
	Int64 showTime;
} ListBoxData;

typedef struct
{
	GtkListBoxRow *row;
	GtkWidget *lbl;
	const UTF8Char *txt;
	void *userData;
} ListBoxItem;

void GUIListBox_SelChange(GtkListBox *listBox, GtkListBoxRow *row, gpointer data)
{
	UI::GUIListBox *me = (UI::GUIListBox*)data;
	me->EventSelectionChange();
}

gboolean GUIListBox_ButtonPress(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GUIListBox *me = (UI::GUIListBox*)data;
	if (event->button.type == GDK_2BUTTON_PRESS)
	{
		me->EventDoubleClick();
	}
	else if (event->button.button == 3)
	{
		me->EventRightClick(event->button.x, event->button.y);
	}
	return false;
}

void GUIListBox_Show(GtkWidget *widget, gpointer user_data)
{
	ListBoxData *data = (ListBoxData*)user_data;
	data->isShown = true;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	data->showTime = dt.ToTicks();
}

UI::GUIListBox::GUIListBox(UI::GUICore *ui, UI::GUIClientControl *parent, Bool multiSelect) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->selChgHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->selChgObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->dblClickHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->dblClickObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->rightClickHdlrs, Data::ArrayList<UI::GUIControl::MouseEventHandler>());
	NEW_CLASS(this->rightClickObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->items, Data::ArrayList<void*>());
	this->mulSel = multiSelect;
	ListBoxData *data = MemAlloc(ListBoxData, 1);
	this->clsData = data;
	data->listbox = gtk_list_box_new();
	data->isShown = false;
	data->showTime = 0;
	if (this->mulSel)
	{
		gtk_list_box_set_selection_mode((GtkListBox*)data->listbox, GTK_SELECTION_MULTIPLE);
	}
	this->hwnd = gtk_scrolled_window_new(0, 0);
	gtk_container_add(GTK_CONTAINER(this->hwnd), data->listbox);
	parent->AddChild(this);
	g_signal_connect(data->listbox, "row-selected", G_CALLBACK(GUIListBox_SelChange), this);
	g_signal_connect(data->listbox, "button-press-event", G_CALLBACK(GUIListBox_ButtonPress), this);
	g_signal_connect(data->listbox, "show", G_CALLBACK(GUIListBox_Show), data);
	gtk_widget_show(data->listbox);
	this->Show();
}

UI::GUIListBox::~GUIListBox()
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	ListBoxItem *item;
	OSInt i;
	DEL_CLASS(this->selChgHdlrs);
	DEL_CLASS(this->selChgObjs);
	DEL_CLASS(this->dblClickHdlrs);
	DEL_CLASS(this->dblClickObjs);
	DEL_CLASS(this->rightClickHdlrs);
	DEL_CLASS(this->rightClickObjs);
	i = this->items->GetCount();
	while (i-- > 0)
	{
		item = (ListBoxItem*)this->items->GetItem(i);
		Text::StrDelNew(item->txt);
		MemFree(item);
	}
	DEL_CLASS(this->items);
	MemFree(data);
}

void UI::GUIListBox::EventSelectionChange()
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	if (!data->isShown)
	{
		return;
	}
	if (data->showTime != 0)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		Int64 t = dt.ToTicks();
		if (t - data->showTime < 2000)
		{
			return;
		}
		data->showTime = 0;
	}

	OSInt i = this->selChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs->GetItem(i)(this->selChgObjs->GetItem(i));
	}
}

void UI::GUIListBox::EventDoubleClick()
{
	OSInt i = this->dblClickHdlrs->GetCount();
	while (i-- > 0)
	{
		this->dblClickHdlrs->GetItem(i)(this->dblClickObjs->GetItem(i));
	}
}

void UI::GUIListBox::EventRightClick(OSInt x, OSInt y)
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	OSInt i = this->rightClickHdlrs->GetCount();
	GtkListBoxRow *row = gtk_list_box_get_row_at_y((GtkListBox*)data->listbox, y);
	if (row)
	{
		gtk_list_box_select_row((GtkListBox*)data->listbox, row);
	}
	GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment((GtkScrolledWindow*)this->hwnd);
	y -= (Int32)gtk_adjustment_get_value(adj);
	if (i > 0)
	{
		while (i-- > 0)
		{
			this->rightClickHdlrs->GetItem(i)(this->rightClickObjs->GetItem(i), x, y, UI::GUIControl::MBTN_RIGHT);
		}
	}
}

OSInt UI::GUIListBox::AddItem(const UTF8Char *itemText, void *itemObj)
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	ListBoxItem *item = MemAlloc(ListBoxItem, 1);
	item->row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
	item->userData = itemObj;
	item->txt = Text::StrCopyNew(itemText);
	item->lbl = gtk_label_new((const Char*)itemText);
	gtk_label_set_ellipsize((GtkLabel*)item->lbl, PANGO_ELLIPSIZE_END);
#if GTK_MAJOR_VERSION == 3
#if GTK_MINOR_VERSION >= 16
	gtk_label_set_xalign(GTK_LABEL((GtkWidget*)item->lbl), 0.0);
#elif GTK_MINOR_VERSION < 14
	gtk_misc_set_alignment(GTK_MISC((GtkWidget*)item->lbl), 0.0, 0.0);
#else
	gtk_widget_set_halign((GtkWidget*)item->lbl, GTK_ALIGN_START);
#endif
#endif
	gtk_widget_show(item->lbl);
	gtk_container_add(GTK_CONTAINER(item->row), item->lbl);
	gtk_widget_show((GtkWidget*)item->row);
	gtk_list_box_insert((GtkListBox*)data->listbox, (GtkWidget*)item->row, -1);
	OSInt ret = this->items->Add(item);
	return ret;
}

OSInt UI::GUIListBox::AddItem(const WChar *itemText, void *itemObj)
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	ListBoxItem *item = MemAlloc(ListBoxItem, 1);
	item->row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
	item->userData = itemObj;
	item->txt = Text::StrToUTF8New(itemText);
	item->lbl = gtk_label_new((const Char*)item->txt);
	gtk_label_set_ellipsize((GtkLabel*)item->lbl, PANGO_ELLIPSIZE_END);
#if GTK_MAJOR_VERSION == 3
#if GTK_MINOR_VERSION >= 16
	gtk_label_set_xalign(GTK_LABEL((GtkWidget*)item->lbl), 0.0);
#elif GTK_MINOR_VERSION < 14
	gtk_misc_set_alignment(GTK_MISC((GtkWidget*)item->lbl), 0.0, 0.0);
#else
	gtk_widget_set_halign((GtkWidget*)item->lbl, GTK_ALIGN_START);
#endif
#endif
	gtk_widget_show(item->lbl);
	gtk_container_add(GTK_CONTAINER(item->row), item->lbl);
	gtk_widget_show((GtkWidget*)item->row);
	gtk_list_box_insert((GtkListBox*)data->listbox, (GtkWidget*)item->row, -1);
	OSInt ret = this->items->Add(item);
	return ret;
}

OSInt UI::GUIListBox::InsertItem(OSInt index, const UTF8Char *itemText, void *itemObj)
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	ListBoxItem *item = MemAlloc(ListBoxItem, 1);
	item->row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
	item->userData = itemObj;
	item->txt = Text::StrCopyNew(itemText);
	item->lbl = gtk_label_new((const Char*)itemText);
	gtk_label_set_ellipsize((GtkLabel*)item->lbl, PANGO_ELLIPSIZE_END);
#if GTK_MAJOR_VERSION == 3
#if GTK_MINOR_VERSION >= 16
	gtk_label_set_xalign(GTK_LABEL((GtkWidget*)item->lbl), 0.0);
#elif GTK_MINOR_VERSION < 14
	gtk_misc_set_alignment(GTK_MISC((GtkWidget*)item->lbl), 0.0, 0.0);
#else
	gtk_widget_set_halign((GtkWidget*)item->lbl, GTK_ALIGN_START);
#endif
#endif
	gtk_widget_show(item->lbl);
	gtk_container_add(GTK_CONTAINER(item->row), item->lbl);
	gtk_widget_show((GtkWidget*)item->row);
	gtk_list_box_insert((GtkListBox*)data->listbox, (GtkWidget*)item->row, index);
	OSInt i = gtk_list_box_row_get_index(item->row);
	if (i == -1)
	{
		this->items->Add(item);
	}
	else
	{
		this->items->Insert(i, item);
	}
	return i;
}

OSInt UI::GUIListBox::InsertItem(OSInt index, const WChar *itemText, void *itemObj)
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	ListBoxItem *item = MemAlloc(ListBoxItem, 1);
	item->row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
	item->userData = itemObj;
	item->txt = Text::StrToUTF8New(itemText);
	item->lbl = gtk_label_new((const Char*)item->txt);
	gtk_label_set_ellipsize((GtkLabel*)item->lbl, PANGO_ELLIPSIZE_END);
#if GTK_MAJOR_VERSION == 3
#if GTK_MINOR_VERSION >= 16
	gtk_label_set_xalign(GTK_LABEL((GtkWidget*)item->lbl), 0.0);
#elif GTK_MINOR_VERSION < 14
	gtk_misc_set_alignment(GTK_MISC((GtkWidget*)item->lbl), 0.0, 0.0);
#else
	gtk_widget_set_halign((GtkWidget*)item->lbl, GTK_ALIGN_START);
#endif
#endif
	gtk_widget_show(item->lbl);
	gtk_container_add(GTK_CONTAINER(item->row), item->lbl);
	gtk_widget_show((GtkWidget*)item->row);
	gtk_list_box_insert((GtkListBox*)data->listbox, (GtkWidget*)item->row, index);
	OSInt i = gtk_list_box_row_get_index(item->row);
	if (i == -1)
	{
		this->items->Add(item);
	}
	else
	{
		this->items->Insert(i, item);
	}
	return i;
}

void *UI::GUIListBox::RemoveItem(OSInt index)
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	ListBoxItem *item = (ListBoxItem*)this->items->GetItem(index);
	if (item == 0)
		return 0;
	gtk_container_remove(GTK_CONTAINER(data->listbox), (GtkWidget*)item->row);
	void *ret = item->userData;
	Text::StrDelNew(item->txt);
	MemFree(item);
	this->items->RemoveAt(index);
	return ret;
}

void *UI::GUIListBox::GetItem(OSInt index)
{
	ListBoxItem *item = (ListBoxItem*)this->items->GetItem(index);
	if (item == 0)
		return 0;
	return item->userData;
}

void UI::GUIListBox::ClearItems()
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	GList *list = gtk_container_get_children((GtkContainer*)data->listbox);
	GList *curr = list;
	while (curr)
	{
		gtk_container_remove((GtkContainer*)data->listbox, (GtkWidget*)curr->data);
		curr = curr->next;
	}
	g_list_free(list);
	OSInt i;
	ListBoxItem *item;
	i = this->items->GetCount();
	while (i-- > 0)
	{
		item = (ListBoxItem*)this->items->GetItem(i);
		Text::StrDelNew(item->txt);
		MemFree(item);
	}
	this->items->Clear();
}

OSInt UI::GUIListBox::GetCount()
{
	return this->items->GetCount();
}

void UI::GUIListBox::SetSelectedIndex(OSInt index)
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	ListBoxItem *item = (ListBoxItem*)this->items->GetItem(index);
	if (item == 0)
		return;
	data->isShown = true;
	data->showTime = 0;
	gtk_list_box_select_row((GtkListBox*)data->listbox, item->row);

	int h = gtk_widget_get_allocated_height(data->listbox);
	Double itemH = h / (Double)this->items->GetCount();
	Double targetTop = itemH * index;
	Double targetBottom = targetTop + itemH;
	GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment((GtkScrolledWindow*)this->hwnd);
	Double pageSize = gtk_adjustment_get_page_size(adj);
	Double currVal = gtk_adjustment_get_value(adj);
	if (currVal + pageSize < targetBottom)
	{
		currVal = targetBottom - pageSize;
		gtk_adjustment_set_value(adj, currVal);
	}
	else if (currVal > targetTop)
	{
		currVal = targetTop;
		gtk_adjustment_set_value(adj, currVal);
	}

//	this->EventSelectionChange();
}

OSInt UI::GUIListBox::GetSelectedIndex()
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	GtkListBoxRow *row = gtk_list_box_get_selected_row((GtkListBox*)data->listbox);
	if (row == 0)
		return -1;
	return gtk_list_box_row_get_index(row);
}

Bool UI::GUIListBox::GetSelectedIndices(Data::ArrayList<Int32> *indices)
{
	ListBoxData *data = (ListBoxData*)this->clsData;
	GList *list = gtk_list_box_get_selected_rows((GtkListBox*)data->listbox);
	GList *curr = list;
	while (curr)
	{
		indices->Add(gtk_list_box_row_get_index((GtkListBoxRow*)curr->data));
		curr = curr->next;
	}
	g_list_free(list);
	return true;
}

void *UI::GUIListBox::GetSelectedItem()
{
	OSInt currSel = GetSelectedIndex();
	if (currSel <= -1)
		return 0;
	return GetItem(currSel);
}

UTF8Char *UI::GUIListBox::GetSelectedItemText(UTF8Char *buff)
{
	OSInt currSel = GetSelectedIndex();
	if (currSel <= -1)
		return 0;
	return GetItemText(buff, currSel);
}

WChar *UI::GUIListBox::GetSelectedItemText(WChar *buff)
{
	OSInt currSel = GetSelectedIndex();
	if (currSel <= -1)
		return 0;
	return GetItemText(buff, currSel);
}

const UTF8Char *UI::GUIListBox::GetSelectedItemTextNew()
{
	OSInt currSel = GetSelectedIndex();
	if (currSel <= -1)
		return 0;
	return GetItemTextNew(currSel);
}

UTF8Char *UI::GUIListBox::GetItemText(UTF8Char *buff, OSInt index)
{
	ListBoxItem *item = (ListBoxItem*)this->items->GetItem(index);
	if (item == 0)
		return 0;
	return Text::StrConcat(buff, item->txt);
}

WChar *UI::GUIListBox::GetItemText(WChar *buff, OSInt index)
{
	ListBoxItem *item = (ListBoxItem*)this->items->GetItem(index);
	if (item == 0)
		return 0;
	return Text::StrUTF8_WChar(buff, item->txt, -1, 0);
}

void UI::GUIListBox::SetItemText(OSInt index, const UTF8Char *text)
{
	ListBoxItem *item = (ListBoxItem*)this->items->GetItem(index);
	if (item == 0)
		return;
	gtk_label_set_text((GtkLabel*)item->lbl, (const Char*)text);
	Text::StrDelNew(item->txt);
	item->txt = Text::StrCopyNew(text);
}

const UTF8Char *UI::GUIListBox::GetItemTextNew(OSInt index)
{
	ListBoxItem *item = (ListBoxItem*)this->items->GetItem(index);
	if (item == 0)
		return 0;
	return Text::StrCopyNew(item->txt);
}

void UI::GUIListBox::DelTextNew(const UTF8Char *text)
{
	Text::StrDelNew(text);
}

OSInt UI::GUIListBox::GetItemHeight()
{
	return 24;
}

const UTF8Char *UI::GUIListBox::GetObjectClass()
{
	return (const UTF8Char*)"ListBox";
}

OSInt UI::GUIListBox::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

void UI::GUIListBox::HandleSelectionChange(UI::UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs->Add(hdlr);
	this->selChgObjs->Add(userObj);
}

void UI::GUIListBox::HandleDoubleClicked(UI::UIEvent hdlr, void *userObj)
{
	this->dblClickHdlrs->Add(hdlr);
	this->dblClickObjs->Add(userObj);
}

void UI::GUIListBox::HandleRightClicked(UI::GUIControl::MouseEventHandler hdlr, void *userObj)
{
	this->rightClickHdlrs->Add(hdlr);
	this->rightClickObjs->Add(userObj);
}
