#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIListView.h"
#include <gtk/gtk.h>

typedef struct
{
	GtkTreeIter iter;
	void *data;
	NotNullPtr<Text::String> txt;
} MyRow;

typedef struct
{
	GtkListStore *listStore;
	GtkWidget *treeView;
	UOSInt colCnt;
	Double *colSizes;
	Data::ArrayList<MyRow*> *rows;
	UI::ListViewStyle lvstyle;
	Bool noChgEvt;
} GUIListViewData;

void GUIListView_SelChange(GtkTreeView *tree_view, gpointer user_data)
{
	UI::GUIListView *me = (UI::GUIListView*)user_data;
	me->EventSelChg();
}

gboolean GUIListView_ButtonClick(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	if (event->type == GDK_DOUBLE_BUTTON_PRESS)
	{
		UI::GUIListView *me = (UI::GUIListView*)user_data;
		UOSInt i = me->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			me->EventDblClk(i);
		}
	}
	else if (event->type == GDK_BUTTON_PRESS)
	{
		UI::GUIListView *me = (UI::GUIListView*)user_data;
		UI::GUIControl::MouseButton btn;
		switch (event->button)
		{
		case GDK_BUTTON_PRIMARY:
			btn = UI::GUIControl::MouseButton::MBTN_LEFT;
			break;
		case GDK_BUTTON_MIDDLE:
			btn = UI::GUIControl::MouseButton::MBTN_MIDDLE;
			break;
		case GDK_BUTTON_SECONDARY:
			btn = UI::GUIControl::MouseButton::MBTN_RIGHT;
			break;
		default:
			btn = UI::GUIControl::MouseButton::MBTN_LEFT;
			break;
		}
		me->EventMouseClick(Math::Coord2DDbl(event->x, event->y), btn);
	}
	return false;
}

void GUIListView_Toggled(GtkCellRendererToggle* self, gchar* path, gpointer user_data)
{
	UI::GUIListView *me = (UI::GUIListView*)user_data;
	me->OnNotify(0x1234, path);
}

UI::GUIListView::GUIListView(NotNullPtr<GUICore> ui, NotNullPtr<GUIClientControl> parent, ListViewStyle lvstyle, UOSInt colCount) : UI::GUIControl(ui, parent)
{
	GUIListViewData *data = MemAlloc(GUIListViewData, 1);
	data->noChgEvt = false;
	UOSInt i;
	GType *types;
	if (lvstyle == UI::ListViewStyle::SmallIcon)
	{
		colCount++;
		types = MemAlloc(GType, colCount);
		data->colSizes = MemAlloc(Double, colCount);
		i = colCount - 1;
		types[i] = G_TYPE_BOOLEAN;
		data->colSizes[i] = 0;
		while (i-- > 0)
		{
			types[i] = G_TYPE_STRING;
			data->colSizes[i] = 0;
		}
	}
	else
	{
		types = MemAlloc(GType, colCount);
		data->colSizes = MemAlloc(Double, colCount);
		i = colCount;
		while (i-- > 0)
		{
			types[i] = G_TYPE_STRING;
			data->colSizes[i] = 0;
		}
	}
	data->listStore = gtk_list_store_newv((gint)colCount, types);
	data->treeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(data->listStore));
	MemFree(types);
	if (lvstyle == UI::ListViewStyle::Table)
	{
		gtk_tree_view_set_headers_clickable((GtkTreeView*)data->treeView, true);
	}
	else if (lvstyle == UI::ListViewStyle::SmallIcon)
	{
		GtkCellRenderer *tickRenderer = gtk_cell_renderer_toggle_new();
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
		GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes("Tick", tickRenderer, "active", 1, (void*)0);
		gtk_cell_renderer_toggle_set_activatable((GtkCellRendererToggle*)tickRenderer, true);
		gtk_tree_view_append_column((GtkTreeView*)data->treeView, col);
		col = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 0, (void*)0);
		gtk_tree_view_append_column((GtkTreeView*)data->treeView, col);
		g_signal_connect(tickRenderer, "toggled", G_CALLBACK(GUIListView_Toggled), this);
	}
	data->colCnt = colCount;
	NEW_CLASS(data->rows, Data::ArrayList<MyRow*>());
	data->lvstyle = lvstyle;
	this->clsData = data;
	this->colCnt = 0;
	this->hwnd = (ControlHandle*)gtk_scrolled_window_new(0, 0);
	gtk_container_add(GTK_CONTAINER(this->hwnd), data->treeView);
	parent->AddChild(*this);
	g_signal_connect(data->treeView, "cursor-changed", G_CALLBACK(GUIListView_SelChange), this);
    g_signal_connect(data->treeView, "button-press-event", G_CALLBACK(GUIListView_ButtonClick), this);
	gtk_widget_show(data->treeView);
	this->Show();
}

UI::GUIListView::~GUIListView()
{
	UOSInt i;
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row;
	i = data->rows->GetCount();
	while (i-- > 0)
	{
		row = data->rows->GetItem(i);
		row->txt->Release();
		MemFree(row);
	}
	MemFree(data->colSizes);
	DEL_CLASS(data->rows);
	MemFree(data);
}

void UI::GUIListView::ChangeColumnCnt(UOSInt newColCnt)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	UOSInt i;
	GType *types;
	MemFree(data->colSizes);
	if (data->lvstyle == UI::ListViewStyle::SmallIcon)
	{
		newColCnt++;
		types = MemAlloc(GType, newColCnt);
		data->colSizes = MemAlloc(Double, newColCnt);
		i = newColCnt - 1;
		types[i] = G_TYPE_BOOLEAN;
		data->colSizes[i] = 0;
		while (i-- > 0)
		{
			types[i] = G_TYPE_STRING;
			data->colSizes[i] = 0;
		}
	}
	else
	{
		types = MemAlloc(GType, newColCnt);
		data->colSizes = MemAlloc(Double, newColCnt);
		i = newColCnt;
		while (i-- > 0)
		{
			types[i] = G_TYPE_STRING;
			data->colSizes[i] = 0;
		}
	}
	data->listStore = gtk_list_store_newv((gint)newColCnt, types);
	gtk_tree_view_set_model((GtkTreeView*)data->treeView, GTK_TREE_MODEL(data->listStore));
	MemFree(types);
	data->colCnt = newColCnt;
}

UOSInt UI::GUIListView::GetColumnCnt()
{
	return this->colCnt;
}

Bool UI::GUIListView::AddColumn(NotNullPtr<Text::String> columnName, Double colWidth)
{
	return this->AddColumn(columnName->ToCString(), colWidth);
}

Bool UI::GUIListView::AddColumn(Text::CStringNN columnName, Double colWidth)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	if (data->colCnt <= this->colCnt)
		return false;

	Text::StringBuilderUTF8 sb;
	sb.Append(columnName);
	sb.ReplaceStr(UTF8STRC("_"), UTF8STRC("__"));
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes((const Char*)sb.v, renderer, "text", this->colCnt, (void*)0);
	gtk_tree_view_column_set_fixed_width(col, Double2Int32(colWidth * this->hdpi / this->ddpi));
	gtk_tree_view_column_set_resizable(col, true);
	gtk_tree_view_append_column((GtkTreeView*)data->treeView, col);
	data->colSizes[this->colCnt] = colWidth;
	this->colCnt++;
	return true;
}

Bool UI::GUIListView::AddColumn(const WChar *columnName, Double colWidth)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	if (data->colCnt <= this->colCnt)
		return false;

	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	const UTF8Char *lbl = Text::StrToUTF8New(columnName);
	GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes((const Char*)lbl, renderer, "text", this->colCnt, (void*)0);
	Text::StrDelNew(lbl);
	gtk_tree_view_column_set_fixed_width(col, Double2Int32(colWidth * this->hdpi / this->ddpi));
	gtk_tree_view_column_set_resizable(col, true);
	gtk_tree_view_append_column((GtkTreeView*)data->treeView, col);
	data->colSizes[this->colCnt] = colWidth;
	this->colCnt++;
	return true;
}

Bool UI::GUIListView::SetColumnWidth(UOSInt index, Double colWidth)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	if (index >= data->colCnt)
		return false;
	GtkTreeViewColumn *col = gtk_tree_view_get_column((GtkTreeView*)data->treeView, (gint)index);
	if (col == 0)
		return false;
	gtk_tree_view_column_set_fixed_width(col, Double2Int32(colWidth * this->hdpi / this->ddpi));
	data->colSizes[index] = colWidth;
	return true;
}

Bool UI::GUIListView::ClearAll()
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	guint cnt = gtk_tree_view_get_n_columns((GtkTreeView *)data->treeView);
	while (cnt-- > 0)
	{
		GtkTreeViewColumn *col = gtk_tree_view_get_column((GtkTreeView*)data->treeView, (gint)cnt);
		gtk_tree_view_remove_column((GtkTreeView*)data->treeView, col);
	}
	this->colCnt = 0;
	this->ClearItems();
	return true;
}

UOSInt UI::GUIListView::AddItem(NotNullPtr<Text::String> text, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = text->Clone();
	gtk_list_store_append(data->listStore, &row->iter);
	UOSInt ret = data->rows->Add(row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)text->v, -1);
	return ret;
}

UOSInt UI::GUIListView::AddItem(Text::CStringNN text, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = Text::String::New(text);
	gtk_list_store_append(data->listStore, &row->iter);
	UOSInt ret = data->rows->Add(row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)text.v, -1);
	return ret;
}

UOSInt UI::GUIListView::AddItem(const WChar *text, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = Text::String::NewNotNull(text);
	gtk_list_store_append(data->listStore, &row->iter);
	UOSInt ret = data->rows->Add(row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)row->txt->v, -1);
	return ret;
}

UOSInt UI::GUIListView::AddItem(Text::CStringNN text, void *itemObj, UOSInt imageIndex)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = Text::String::New(text);
	gtk_list_store_append(data->listStore, &row->iter);
	UOSInt ret = data->rows->Add(row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)text.v, -1);
	return ret;
}

Bool UI::GUIListView::SetSubItem(UOSInt row, UOSInt col, NotNullPtr<Text::String> text)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(row);
	if (r == 0 || col < 0 || col >= data->colCnt)
		return false;
	gtk_list_store_set(data->listStore, &r->iter, col, (const Char*)text->v, -1);
	return true;
}

Bool UI::GUIListView::SetSubItem(UOSInt row, UOSInt col, Text::CStringNN text)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(row);
	if (r == 0 || col < 0 || col >= data->colCnt)
		return false;
	gtk_list_store_set(data->listStore, &r->iter, col, (const Char*)text.v, -1);
	return true;
}

Bool UI::GUIListView::SetSubItem(UOSInt row, UOSInt col, const WChar *text)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(row);
	if (r == 0 || col >= data->colCnt)
		return false;
	const UTF8Char *txt = Text::StrToUTF8New(text);
	gtk_list_store_set(data->listStore, &r->iter, col, (const Char*)txt, -1);
	Text::StrDelNew(txt);
	return true;
}

Bool UI::GUIListView::GetSubItem(UOSInt index, UOSInt subIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0 || subIndex >= data->colCnt)
		return false;
	const Char *txt = 0;
	gtk_tree_model_get((GtkTreeModel*)data->listStore, &r->iter, subIndex, &txt, -1);
	if (txt)
	{
		sb->AppendSlow((const UTF8Char*)txt);
		g_free((void*)txt);
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt UI::GUIListView::InsertItem(UOSInt index, NotNullPtr<Text::String> itemText, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = itemText->Clone();
	gtk_list_store_insert(data->listStore, &row->iter, (gint)index);
	data->rows->Insert(index, row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)itemText->v, -1);
	return index;
}

UOSInt UI::GUIListView::InsertItem(UOSInt index, Text::CStringNN itemText, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = Text::String::New(itemText);
	gtk_list_store_insert(data->listStore, &row->iter, (gint)index);
	data->rows->Insert(index, row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)itemText.v, -1);
	return index;
}

UOSInt UI::GUIListView::InsertItem(UOSInt index, const WChar *itemText, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = Text::String::NewNotNull(itemText);
	gtk_list_store_insert(data->listStore, &row->iter, (gint)index);
	data->rows->Insert(index, row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)row->txt->v, -1);
	return index;
}

void *UI::GUIListView::RemoveItem(UOSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->RemoveAt(index);
	if (r == 0)
		return 0;

	gtk_list_store_remove(data->listStore, &r->iter);
	void *ret = r->data;
	r->txt->Release();
	MemFree(r);
	return ret;
}

void *UI::GUIListView::GetItem(UOSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return 0;
	return r->data;
}

void UI::GUIListView::ClearItems()
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	UOSInt i;
	MyRow *row;
	data->noChgEvt = true;
	gtk_list_store_clear(data->listStore);
	i = data->rows->GetCount();
	while (i-- > 0)
	{
		row = data->rows->GetItem(i);
		row->txt->Release();
		MemFree(row);
	}
	data->rows->Clear();
	data->noChgEvt = false;
}

UOSInt UI::GUIListView::GetCount()
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	return data->rows->GetCount();
}

void UI::GUIListView::SetSelectedIndex(UOSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	if (index == (UOSInt)-1)
	{
		//////////////////////
		return;
	}
	MyRow *r = data->rows->GetItem((UOSInt)index);
	if (r == 0)
		return;
	GtkTreeSelection *sel = gtk_tree_view_get_selection((GtkTreeView*)data->treeView);
	gtk_tree_selection_select_iter(sel, &r->iter);
	this->EventSelChg();
}

UOSInt UI::GUIListView::GetSelectedIndex()
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	GtkTreeSelection *sel = gtk_tree_view_get_selection((GtkTreeView*)data->treeView);
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(sel, 0, &iter))
	{
		GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(data->listStore), &iter);
		int *i = gtk_tree_path_get_indices(path);
		return (UOSInt)(OSInt)i[0];
	}
	else
	{
		return INVALID_INDEX;
	}
}

UOSInt UI::GUIListView::GetSelectedIndices(Data::ArrayList<UOSInt> *selIndices)
{
	UOSInt ret = 0;
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	GtkTreeSelection *sel = gtk_tree_view_get_selection((GtkTreeView*)data->treeView);
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(sel, 0, &iter))
	{
		GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(data->listStore), &iter);
		gint depth;
		gint *i = gtk_tree_path_get_indices_with_depth(path, &depth);
		ret = 0;
		while (ret < (UOSInt)depth)
		{
			selIndices->Add((UOSInt)i[ret]);
			ret++;
		}
		return ret;
	}
	else
	{
		return ret;
	}
}

void *UI::GUIListView::GetSelectedItem()
{
	UOSInt i = GetSelectedIndex();
	if (i != INVALID_INDEX)
		return this->GetItem(i);
	return 0;
}

UTF8Char *UI::GUIListView::GetSelectedItemText(UTF8Char *buff)
{
	UOSInt i = GetSelectedIndex();
	if (i != INVALID_INDEX)
		return this->GetItemText(buff, i);
	return 0;
}

Text::String *UI::GUIListView::GetSelectedItemTextNew()
{
	UOSInt i = GetSelectedIndex();
	if (i != INVALID_INDEX)
		return this->GetItemTextNew(i);
	return 0;
}

UTF8Char *UI::GUIListView::GetItemText(UTF8Char *buff, UOSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return 0;
	return Text::StrConcatC(buff, r->txt->v, r->txt->leng);
}

Text::String *UI::GUIListView::GetItemTextNew(UOSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return 0;
	return r->txt->Clone().Ptr();
}

void UI::GUIListView::SetFullRowSelect(Bool fullRowSelect)
{
//	SendMessage((HWND)this->hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, fullRowSelect?LVS_EX_FULLROWSELECT:0);
}

void UI::GUIListView::SetShowGrid(Bool showGrid)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	gtk_tree_view_set_grid_lines((GtkTreeView*)data->treeView, showGrid?GTK_TREE_VIEW_GRID_LINES_BOTH:GTK_TREE_VIEW_GRID_LINES_NONE);
}

UOSInt UI::GUIListView::GetStringWidth(const UTF8Char *s)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	UOSInt ret = 0;
	int width;
	int height;
	PangoLayout *layout = gtk_widget_create_pango_layout(data->treeView, (const Char*)s);
	pango_layout_get_pixel_size(layout, &width, &height);
	ret = (UOSInt)width;
	g_object_unref(G_OBJECT(layout));
	return ret;
}

UOSInt UI::GUIListView::GetStringWidth(const WChar *s)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	UOSInt ret = 0;
	int width;
	int height;
	const UTF8Char *csptr = Text::StrToUTF8New(s);
	PangoLayout *layout = gtk_widget_create_pango_layout(data->treeView, (const Char*)csptr);
	pango_layout_get_pixel_size(layout, &width, &height);
	ret = (UOSInt)width;
	g_object_unref(G_OBJECT(layout));
	Text::StrDelNew(csptr);
	return ret;
}

void UI::GUIListView::GetItemRectP(UOSInt index, Int32 *rect)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(data->listStore), &r->iter);
	GdkRectangle rc;
	Math::Size2D<UOSInt> sz = this->GetSizeP();
	gtk_tree_view_get_cell_area((GtkTreeView*)data->treeView, path, 0, &rc);
	rect[0] = 0;
	rect[1] = rc.y;
	rect[2] = (Int32)sz.x;
	rect[3] = rc.height;
}

void UI::GUIListView::EnsureVisible(UOSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(data->listStore), &r->iter);
	gtk_tree_view_scroll_to_cell((GtkTreeView*)data->treeView, path, 0, false, 0, 0);
}

void *UI::GUIListView::SetItem(UOSInt index, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return itemObj;
	void *oldData = r->data;
	r->data = itemObj;
	return oldData;
}

void UI::GUIListView::BeginUpdate()
{
//	this->SetVisible(false);
}

void UI::GUIListView::EndUpdate()
{
//	this->SetVisible(true);
}

Text::CStringNN UI::GUIListView::GetObjectClass() const
{
	return CSTR("ListView");
}

OSInt UI::GUIListView::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUIListView::OnSizeChanged(Bool updateScn)
{
/*	if (this->lvStyle == UI::GUIListView::LVSTYLE_ICON)
	{
		SendMessage((HWND)this->hwnd, LVM_ARRANGE, LVA_DEFAULT, 0);
	}*/
	UOSInt i = this->resizeHandlers.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UI::UIEvent> cb = this->resizeHandlers.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUIListView::EventSelChg()
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	if (data->noChgEvt)
		return;
	UOSInt i;
	i = this->selChgHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<UIEvent> cb = this->selChgHdlrs.GetItem(i);
		cb.func(cb.userObj);
	}
}

void UI::GUIListView::EventDblClk(UOSInt itemIndex)
{
	UOSInt i;
	i = this->dblClkHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<ItemEvent> cb = this->dblClkHdlrs.GetItem(i);
		cb.func(cb.userObj, itemIndex);
	}
}

void UI::GUIListView::EventMouseClick(Math::Coord2DDbl coord, MouseButton btn)
{
	if (btn == MouseButton::MBTN_RIGHT)
	{
		UOSInt index = this->GetSelectedIndex();
		UOSInt i;
		i = this->rClkHdlrs.GetCount();
		while (i-- > 0)
		{
			Data::CallbackStorage<MouseEvent> cb = this->rClkHdlrs.GetItem(i);
			cb.func(cb.userObj, coord, index);
		}
	}
}

void UI::GUIListView::SetDPI(Double hdpi, Double ddpi)
{
	this->hdpi = hdpi;
	this->ddpi = ddpi;
	this->UpdatePos(true);
	if (this->hFont)
	{
		this->InitFont();
	}
	else
	{
		this->UpdateFont();
	}
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	UOSInt i = 0;
	while (i < this->colCnt)
	{
		GtkTreeViewColumn *col = gtk_tree_view_get_column((GtkTreeView*)data->treeView, (gint)i);
		if (col)
		{
			gtk_tree_view_column_set_fixed_width(col, Double2Int32(data->colSizes[i] * this->hdpi / this->ddpi));
		}
		i++;
	}
}

void UI::GUIListView::HandleSelChg(UI::UIEvent hdlr, AnyType userObj)
{
	this->selChgHdlrs.Add({hdlr, userObj});
}

void UI::GUIListView::HandleDblClk(ItemEvent hdlr, AnyType userObj)
{
	this->dblClkHdlrs.Add({hdlr, userObj});
}

void UI::GUIListView::HandleRightClick(MouseEvent hdlr, AnyType userObj)
{
	this->rClkHdlrs.Add({hdlr, userObj});
}
