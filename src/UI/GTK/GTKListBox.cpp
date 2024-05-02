#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GTK/GTKListBox.h"

struct UI::GTK::GTKListBox::ItemData
{
	GtkListBoxRow *row;
	GtkWidget *lbl;
	NN<Text::String> txt;
	AnyType userData;
};

void UI::GTK::GTKListBox::SignalSelChange(GtkListBox *listBox, GtkListBoxRow *row, gpointer data)
{
	UI::GTK::GTKListBox *me = (UI::GTK::GTKListBox*)data;
	if (!me->isShown)
	{
		return;
	}
	if (me->showTime != 0)
	{
		Int64 t = Data::DateTimeUtil::GetCurrTimeMillis();
		if (t - me->showTime < 2000)
		{
			return;
		}
		me->showTime = 0;
	}
	me->EventSelectionChange();
}

gboolean UI::GTK::GTKListBox::SignalButtonPress(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	UI::GTK::GTKListBox *me = (UI::GTK::GTKListBox*)data;
	me->showTime = 0;
	if (event->button.type == GDK_2BUTTON_PRESS)
	{
		me->EventDoubleClick();
	}
	else if (event->button.button == 3)
	{
		GtkListBoxRow *row = gtk_list_box_get_row_at_y((GtkListBox*)me->listbox, (gint)event->button.y);
		if (row)
		{
			gtk_list_box_select_row((GtkListBox*)me->listbox, row);
		}
		GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment((GtkScrolledWindow*)me->hwnd);
		event->button.y -= (Int32)gtk_adjustment_get_value(adj);
		me->EventRightClick(Math::Coord2D<OSInt>(Double2OSInt(event->button.x), Double2OSInt(event->button.y)));
	}
	return false;
}

void UI::GTK::GTKListBox::SignalShow(GtkWidget *widget, gpointer user_data)
{
	UI::GTK::GTKListBox *me = (UI::GTK::GTKListBox*)user_data;
	me->isShown = true;
	me->showTime = Data::DateTimeUtil::GetCurrTimeMillis();
}

UI::GTK::GTKListBox::GTKListBox(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Bool multiSelect) : UI::GUIListBox(ui, parent)
{
	this->mulSel = multiSelect;
	this->listbox = gtk_list_box_new();
	this->isShown = false;
	this->showTime = 0;
	if (this->mulSel)
	{
		gtk_list_box_set_selection_mode((GtkListBox*)this->listbox, GTK_SELECTION_MULTIPLE);
	}
	this->hwnd = (ControlHandle*)gtk_scrolled_window_new(0, 0);
	gtk_container_add(GTK_CONTAINER(this->hwnd), this->listbox);
	parent->AddChild(*this);
	g_signal_connect(this->listbox, "row-selected", G_CALLBACK(SignalSelChange), this);
	g_signal_connect(this->listbox, "button-press-event", G_CALLBACK(SignalButtonPress), this);
	g_signal_connect(this->listbox, "show", G_CALLBACK(SignalShow), this);
	gtk_widget_show(this->listbox);
	this->Show();
}

UI::GTK::GTKListBox::~GTKListBox()
{
	NN<ItemData> item;
	UOSInt i;
	i = this->items.GetCount();
	while (i-- > 0)
	{
		item = this->items.GetItemNoCheck(i);
		item->txt->Release();
		MemFreeNN(item);
	}
}

UOSInt UI::GTK::GTKListBox::AddItem(NN<Text::String> itemText, AnyType itemObj)
{
	NN<ItemData> item = MemAllocNN(ItemData);
	item->row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
	item->userData = itemObj;
	item->txt = itemText->Clone();
	item->lbl = gtk_label_new((const Char*)itemText->v);
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
	gtk_list_box_insert((GtkListBox*)this->listbox, (GtkWidget*)item->row, -1);
	UOSInt ret = this->items.Add(item);
	return ret;
}

UOSInt UI::GTK::GTKListBox::AddItem(Text::CStringNN itemText, AnyType itemObj)
{
	NN<ItemData> item = MemAllocNN(ItemData);
	item->row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
	item->userData = itemObj;
	item->txt = Text::String::New(itemText.v, itemText.leng);
	item->lbl = gtk_label_new((const Char*)itemText.v);
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
	gtk_container_add(GTK_CONTAINER(item->row), item->lbl);
	gtk_widget_show(item->lbl);
	gtk_widget_show((GtkWidget*)item->row);
	gtk_list_box_insert((GtkListBox*)this->listbox, (GtkWidget*)item->row, -1);
	UOSInt ret = this->items.Add(item);
	return ret;
}

UOSInt UI::GTK::GTKListBox::InsertItem(UOSInt index, Text::String *itemText, AnyType itemObj)
{
	NN<ItemData> item = MemAllocNN(ItemData);
	item->row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
	item->userData = itemObj;
	item->txt = itemText->Clone();
	item->lbl = gtk_label_new((const Char*)itemText->v);
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
	gtk_list_box_insert((GtkListBox*)this->listbox, (GtkWidget*)item->row, (gint)(OSInt)index);
	OSInt i = gtk_list_box_row_get_index(item->row);
	if (i == -1)
	{
		i = (OSInt)this->items.Add(item);
	}
	else
	{
		this->items.Insert((UOSInt)i, item);
	}
	return (UOSInt)i;
}

UOSInt UI::GTK::GTKListBox::InsertItem(UOSInt index, Text::CStringNN itemText, AnyType itemObj)
{
	NN<ItemData> item = MemAllocNN(ItemData);
	item->row = GTK_LIST_BOX_ROW(gtk_list_box_row_new());
	item->userData = itemObj;
	item->txt = Text::String::New(itemText.v, itemText.leng);
	item->lbl = gtk_label_new((const Char*)itemText.v);
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
	gtk_list_box_insert((GtkListBox*)this->listbox, (GtkWidget*)item->row, (gint)(OSInt)index);
	OSInt i = gtk_list_box_row_get_index(item->row);
	if (i == -1)
	{
		i = (OSInt)this->items.Add(item);
	}
	else
	{
		this->items.Insert((UOSInt)i, item);
	}
	return (UOSInt)i;
}

AnyType UI::GTK::GTKListBox::RemoveItem(UOSInt index)
{
	NN<ItemData> item;
	if (!this->items.GetItem(index).SetTo(item))
		return 0;
	gtk_container_remove(GTK_CONTAINER(this->listbox), (GtkWidget*)item->row);
	AnyType ret = item->userData;
	item->txt->Release();
	MemFreeNN(item);
	this->items.RemoveAt(index);
	return ret;
}

AnyType UI::GTK::GTKListBox::GetItem(UOSInt index)
{
	NN<ItemData> item;
	if (!this->items.GetItem(index).SetTo(item))
		return 0;
	return item->userData;
}

void UI::GTK::GTKListBox::ClearItems()
{
	GList *list = gtk_container_get_children((GtkContainer*)this->listbox);
	GList *curr = list;
	while (curr)
	{
		gtk_container_remove((GtkContainer*)this->listbox, (GtkWidget*)curr->data);
		curr = curr->next;
	}
	g_list_free(list);
	UOSInt i;
	NN<ItemData> item;
	i = this->items.GetCount();
	while (i-- > 0)
	{
		item = this->items.GetItemNoCheck(i);
		item->txt->Release();
		MemFreeNN(item);
	}
	this->items.Clear();
}

UOSInt UI::GTK::GTKListBox::GetCount()
{
	return this->items.GetCount();
}

void UI::GTK::GTKListBox::SetSelectedIndex(UOSInt index)
{
	NN<ItemData> item;
	if (!this->items.GetItem(index).SetTo(item))
		return;
	this->isShown = true;
	this->showTime = 0;
	gtk_list_box_select_row((GtkListBox*)this->listbox, item->row);

	int h = gtk_widget_get_allocated_height(this->listbox);
	Double itemH = h / (Double)this->items.GetCount();
	Double targetTop = itemH * UOSInt2Double(index);
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

UOSInt UI::GTK::GTKListBox::GetSelectedIndex()
{
	GtkListBoxRow *row = gtk_list_box_get_selected_row((GtkListBox*)this->listbox);
	if (row == 0)
		return INVALID_INDEX;
	return (UInt32)gtk_list_box_row_get_index(row);
}

Bool UI::GTK::GTKListBox::GetSelectedIndices(Data::ArrayList<UInt32> *indices)
{
	GList *list = gtk_list_box_get_selected_rows((GtkListBox*)this->listbox);
	GList *curr = list;
	while (curr)
	{
		indices->Add((UInt32)gtk_list_box_row_get_index((GtkListBoxRow*)curr->data));
		curr = curr->next;
	}
	g_list_free(list);
	return true;
}

AnyType UI::GTK::GTKListBox::GetSelectedItem()
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItem(currSel);
}

UTF8Char *UI::GTK::GTKListBox::GetSelectedItemText(UTF8Char *buff)
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItemText(buff, currSel);
}

Optional<Text::String> UI::GTK::GTKListBox::GetSelectedItemTextNew()
{
	UOSInt currSel = GetSelectedIndex();
	if (currSel == INVALID_INDEX)
		return 0;
	return GetItemTextNew(currSel);
}

UTF8Char *UI::GTK::GTKListBox::GetItemText(UTF8Char *buff, UOSInt index)
{
	NN<ItemData> item;
	if (!this->items.GetItem(index).SetTo(item))
		return 0;
	return Text::StrConcatC(buff, item->txt->v, item->txt->leng);
}

void UI::GTK::GTKListBox::SetItemText(UOSInt index, Text::CStringNN text)
{
	NN<ItemData> item;
	if (!this->items.GetItem(index).SetTo(item))
		return;
	gtk_label_set_text((GtkLabel*)item->lbl, (const Char*)text.v);
	item->txt->Release();
	item->txt = Text::String::New(text.v, text.leng);
}

Optional<Text::String> UI::GTK::GTKListBox::GetItemTextNew(UOSInt index)
{
	NN<ItemData> item;
	if (!this->items.GetItem(index).SetTo(item))
		return 0;
	return item->txt->Clone();
}

OSInt UI::GTK::GTKListBox::GetItemHeight()
{
	return 24;
}

OSInt UI::GTK::GTKListBox::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}
