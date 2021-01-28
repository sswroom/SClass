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
	const UTF8Char *txt;
} MyRow;

typedef struct
{
	GtkListStore *listStore;
	GtkWidget *treeView;
	OSInt colCnt;
	Double *colSizes;
	Data::ArrayList<MyRow*> *rows;
	UI::GUIListView::ListViewStyle lvstyle;
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
		OSInt i = me->GetSelectedIndex();
		if (i >= 0)
		{
			me->EventDblClk(i);
		}
	}
	return false;
}

UI::GUIListView::GUIListView(GUICore *ui, GUIClientControl *parent, ListViewStyle lvstyle, OSInt colCount) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->selChgHdlrs, Data::ArrayList<UI::UIEvent>());
	NEW_CLASS(this->selChgObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->dblClkHdlrs, Data::ArrayList<ItemEvent>());
	NEW_CLASS(this->dblClkObjs, Data::ArrayList<void*>());

	GUIListViewData *data = MemAlloc(GUIListViewData, 1);
	data->noChgEvt = false;
	OSInt i;
	GType *types;
	if (lvstyle == UI::GUIListView::LVSTYLE_SMALLICON)
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
	data->listStore = gtk_list_store_newv(colCount, types);
	data->treeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(data->listStore));
	MemFree(types);
	if (lvstyle == UI::GUIListView::LVSTYLE_TABLE)
	{
		gtk_tree_view_set_headers_clickable((GtkTreeView*)data->treeView, true);
	}
	data->colCnt = colCount;
	NEW_CLASS(data->rows, Data::ArrayList<MyRow*>());
	data->lvstyle = lvstyle;
	this->clsData = data;
	this->colCnt = 0;
	this->hwnd = gtk_scrolled_window_new(0, 0);
	gtk_container_add(GTK_CONTAINER(this->hwnd), data->treeView);
	parent->AddChild(this);
	g_signal_connect(data->treeView, "cursor-changed", G_CALLBACK(GUIListView_SelChange), this);
    g_signal_connect(data->treeView, "button-press-event", G_CALLBACK(GUIListView_ButtonClick), this);
	gtk_widget_show(data->treeView);
	this->Show();
}

UI::GUIListView::~GUIListView()
{
	OSInt i;
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row;
	i = data->rows->GetCount();
	while (i-- > 0)
	{
		row = data->rows->GetItem(i);
		Text::StrDelNew(row->txt);
		MemFree(row);
	}
	MemFree(data->colSizes);
	DEL_CLASS(data->rows);
	MemFree(data);
	DEL_CLASS(this->selChgHdlrs);
	DEL_CLASS(this->selChgObjs);
	DEL_CLASS(this->dblClkHdlrs);
	DEL_CLASS(this->dblClkObjs);
}

void UI::GUIListView::ChangeColumnCnt(OSInt newColCnt)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	OSInt i;
	GType *types;
	MemFree(data->colSizes);
	if (data->lvstyle == UI::GUIListView::LVSTYLE_SMALLICON)
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
	data->listStore = gtk_list_store_newv(newColCnt, types);
	gtk_tree_view_set_model((GtkTreeView*)data->treeView, GTK_TREE_MODEL(data->listStore));
	MemFree(types);
	data->colCnt = newColCnt;
}

OSInt UI::GUIListView::GetColumnCnt()
{
	return this->colCnt;
}

Bool UI::GUIListView::AddColumn(const UTF8Char *columnName, Double colWidth)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	if (data->colCnt <= this->colCnt)
		return false;

	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes((const Char*)columnName, renderer, "text", this->colCnt, (void*)0);
	gtk_tree_view_column_set_fixed_width(col, Math::Double2Int32(colWidth * this->hdpi / this->ddpi));
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
	gtk_tree_view_column_set_fixed_width(col, Math::Double2Int32(colWidth * this->hdpi / this->ddpi));
	gtk_tree_view_column_set_resizable(col, true);
	gtk_tree_view_append_column((GtkTreeView*)data->treeView, col);
	data->colSizes[this->colCnt] = colWidth;
	this->colCnt++;
	return true;
}

Bool UI::GUIListView::SetColumnWidth(OSInt index, Double colWidth)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	if (index < 0 || index >= data->colCnt)
		return false;
	GtkTreeViewColumn *col = gtk_tree_view_get_column((GtkTreeView*)data->treeView, index);
	if (col == 0)
		return false;
	gtk_tree_view_column_set_fixed_width(col, Math::Double2Int32(colWidth * this->hdpi / this->ddpi));
	data->colSizes[index] = colWidth;
	return true;
}

Bool UI::GUIListView::ClearAll()
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	guint cnt = gtk_tree_view_get_n_columns((GtkTreeView *)data->treeView);
	while (cnt-- > 0)
	{
		GtkTreeViewColumn *col = gtk_tree_view_get_column((GtkTreeView*)data->treeView, cnt);
		gtk_tree_view_remove_column((GtkTreeView*)data->treeView, col);
	}
	this->colCnt = 0;
	this->ClearItems();
	return true;
}

OSInt UI::GUIListView::AddItem(const UTF8Char *text, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = Text::StrCopyNew(text);
	gtk_list_store_append(data->listStore, &row->iter);
	OSInt ret = data->rows->Add(row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)text, -1);
	return ret;
}

OSInt UI::GUIListView::AddItem(const WChar *text, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = Text::StrToUTF8New(text);
	gtk_list_store_append(data->listStore, &row->iter);
	OSInt ret = data->rows->Add(row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)row->txt, -1);
	return ret;
}

OSInt UI::GUIListView::AddItem(const UTF8Char *text, void *itemObj, OSInt imageIndex)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = Text::StrCopyNew(text);
	gtk_list_store_append(data->listStore, &row->iter);
	OSInt ret = data->rows->Add(row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)text, -1);
	return ret;
}

Bool UI::GUIListView::SetSubItem(OSInt row, OSInt col, const UTF8Char *text)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(row);
	if (r == 0 || col < 0 || col >= data->colCnt)
		return false;
	gtk_list_store_set(data->listStore, &r->iter, col, (const Char*)text, -1);
	return true;
}

Bool UI::GUIListView::SetSubItem(OSInt row, OSInt col, const WChar *text)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(row);
	if (r == 0 || col < 0 || col >= data->colCnt)
		return false;
	const UTF8Char *txt = Text::StrToUTF8New(text);
	gtk_list_store_set(data->listStore, &r->iter, col, (const Char*)txt, -1);
	Text::StrDelNew(txt);
	return true;
}

Bool UI::GUIListView::GetSubItem(OSInt index, OSInt subIndex, Text::StringBuilderUTF *sb)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0 || subIndex < 0 || subIndex >= data->colCnt)
		return false;
	const Char *txt = 0;
	gtk_tree_model_get((GtkTreeModel*)data->listStore, &r->iter, subIndex, &txt, -1);
	if (txt)
	{
		sb->Append((const UTF8Char*)txt);
		g_free((void*)txt);
		return true;
	}
	else
	{
		return false;
	}
}

OSInt UI::GUIListView::InsertItem(OSInt index, const UTF8Char *itemText, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = Text::StrCopyNew(itemText);
	gtk_list_store_insert(data->listStore, &row->iter, index);
	data->rows->Insert(index, row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)itemText, -1);
	return index;
}

OSInt UI::GUIListView::InsertItem(OSInt index, const WChar *itemText, void *itemObj)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *row = MemAlloc(MyRow, 1);
	row->data = itemObj;
	row->txt = Text::StrToUTF8New(itemText);
	gtk_list_store_insert(data->listStore, &row->iter, index);
	data->rows->Insert(index, row);
	gtk_list_store_set(data->listStore, &row->iter, 0, (const Char*)row->txt, -1);
	return index;
}

void *UI::GUIListView::RemoveItem(OSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->RemoveAt(index);
	if (r == 0)
		return 0;

	gtk_list_store_remove(data->listStore, &r->iter);
	void *ret = r->data;
	Text::StrDelNew(r->txt);
	MemFree(r);
	return ret;
}

void *UI::GUIListView::GetItem(OSInt index)
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
	OSInt i;
	MyRow *row;
	data->noChgEvt = true;
	gtk_list_store_clear(data->listStore);
	i = data->rows->GetCount();
	while (i-- > 0)
	{
		row = data->rows->GetItem(i);
		Text::StrDelNew(row->txt);
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

void UI::GUIListView::SetSelectedIndex(OSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return;
	GtkTreeSelection *sel = gtk_tree_view_get_selection((GtkTreeView*)data->treeView);
	gtk_tree_selection_select_iter(sel, &r->iter);
	this->EventSelChg();
}

OSInt UI::GUIListView::GetSelectedIndex()
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	GtkTreeSelection *sel = gtk_tree_view_get_selection((GtkTreeView*)data->treeView);
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(sel, 0, &iter))
	{
		GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(data->listStore), &iter);
		int *i = gtk_tree_path_get_indices(path);
		return i[0];
	}
	else
	{
		return -1;
	}
}

OSInt UI::GUIListView::GetSelectedIndices(Data::ArrayList<OSInt> *selIndices)
{
	OSInt ret = 0;
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	GtkTreeSelection *sel = gtk_tree_view_get_selection((GtkTreeView*)data->treeView);
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(sel, 0, &iter))
	{
		GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(data->listStore), &iter);
		gint depth;
		gint *i = gtk_tree_path_get_indices_with_depth(path, &depth);
		ret = 0;
		while (ret < depth)
		{
			selIndices->Add(i[ret]);
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
	OSInt i = GetSelectedIndex();
	if (i >= 0)
		return this->GetItem(i);
	return 0;
}

UTF8Char *UI::GUIListView::GetSelectedItemText(UTF8Char *buff)
{
	OSInt i = GetSelectedIndex();
	if (i >= 0)
		return this->GetItemText(buff, i);
	return 0;
}

const UTF8Char *UI::GUIListView::GetSelectedItemTextNew()
{
	OSInt i = GetSelectedIndex();
	if (i >= 0)
		return this->GetItemTextNew(i);
	return 0;
}

UTF8Char *UI::GUIListView::GetItemText(UTF8Char *buff, OSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return 0;
	return Text::StrConcat(buff, r->txt);
}

const UTF8Char *UI::GUIListView::GetItemTextNew(OSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return 0;
	return Text::StrCopyNew(r->txt);
}

void UI::GUIListView::DelTextNew(const UTF8Char *text)
{
	Text::StrDelNew(text);
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

OSInt UI::GUIListView::GetStringWidth(const UTF8Char *s)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	OSInt ret = 0;
	int width;
	int height;
	PangoLayout *layout = gtk_widget_create_pango_layout(data->treeView, (const Char*)s);
	pango_layout_get_pixel_size(layout, &width, &height);
	ret = width;
	g_object_unref(G_OBJECT(layout));
	return ret;
}

OSInt UI::GUIListView::GetStringWidth(const WChar *s)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	OSInt ret = 0;
	int width;
	int height;
	const UTF8Char *csptr = Text::StrToUTF8New(s);
	PangoLayout *layout = gtk_widget_create_pango_layout(data->treeView, (const Char*)csptr);
	pango_layout_get_pixel_size(layout, &width, &height);
	ret = width;
	g_object_unref(G_OBJECT(layout));
	Text::StrDelNew(csptr);
	return ret;
}

void UI::GUIListView::GetItemRectP(OSInt index, Int32 *rect)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(data->listStore), &r->iter);
	GdkRectangle rc;
	UOSInt w;
	UOSInt h;
	this->GetSizeP(&w, &h);
	gtk_tree_view_get_cell_area((GtkTreeView*)data->treeView, path, 0, &rc);
	rect[0] = 0;
	rect[1] = rc.y;
	rect[2] = (Int32)w;
	rect[3] = rc.height;
}

void UI::GUIListView::EnsureVisible(OSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(data->listStore), &r->iter);
	gtk_tree_view_scroll_to_cell((GtkTreeView*)data->treeView, path, 0, false, 0, 0);
}

void UI::GUIListView::BeginUpdate()
{
//	this->SetVisible(false);
}

void UI::GUIListView::EndUpdate()
{
//	this->SetVisible(true);
}

const UTF8Char *UI::GUIListView::GetObjectClass()
{
	return (const UTF8Char*)"ListView";
}

OSInt UI::GUIListView::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

void UI::GUIListView::OnSizeChanged(Bool updateScn)
{
/*	if (this->lvStyle == UI::GUIListView::LVSTYLE_ICON)
	{
		SendMessage((HWND)this->hwnd, LVM_ARRANGE, LVA_DEFAULT, 0);
	}*/
	OSInt i = this->resizeHandlers->GetCount();
	while (i-- > 0)
	{
		this->resizeHandlers->GetItem(i)(this->resizeHandlersObjs->GetItem(i));
	}
}

void UI::GUIListView::EventSelChg()
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	if (data->noChgEvt)
		return;
	OSInt i;
	i = this->selChgHdlrs->GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs->GetItem(i)(this->selChgObjs->GetItem(i));
	}
}

void UI::GUIListView::EventDblClk(OSInt itemIndex)
{
	OSInt i;
	i = this->dblClkHdlrs->GetCount();
	while (i-- > 0)
	{
		this->dblClkHdlrs->GetItem(i)(this->dblClkObjs->GetItem(i), itemIndex);
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
	OSInt i = 0;
	while (i < this->colCnt)
	{
		GtkTreeViewColumn *col = gtk_tree_view_get_column((GtkTreeView*)data->treeView, i);
		if (col)
		{
			gtk_tree_view_column_set_fixed_width(col, Math::Double2Int32(data->colSizes[i] * this->hdpi / this->ddpi));
		}
		i++;
	}
}

void UI::GUIListView::HandleSelChg(UI::UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs->Add(hdlr);
	this->selChgObjs->Add(userObj);
}

void UI::GUIListView::HandleDblClk(ItemEvent hdlr, void *userObj)
{
	this->dblClkHdlrs->Add(hdlr);
	this->dblClkObjs->Add(userObj);
}

