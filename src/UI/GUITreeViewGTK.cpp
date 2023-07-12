#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUITreeView.h"
#include "UI/GUIClientControl.h"
#include <gtk/gtk.h>

struct UI::GUITreeView::ClassData
{
	GtkTreeStore *treeStore;
	GtkWidget *treeView;
};

void GUITreeView_SelChange(GtkTreeView *tree_view, gpointer user_data)
{
	UI::GUITreeView *me = (UI::GUITreeView*)user_data;
	me->EventSelectionChange();
}

gboolean GUITreeView_ButtonClick(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	UI::GUITreeView *me = (UI::GUITreeView*)user_data;
	if (event->type == GDK_DOUBLE_BUTTON_PRESS)
	{
		me->EventDoubleClick();
	}
	else if (event->type == GDK_BUTTON_PRESS)
	{
		if (event->button == 3)
		{
			me->EventRightClicked();
		}
	}
	return false;
}

UI::GUITreeView::TreeItem::TreeItem(void *itemObj, NotNullPtr<Text::String> txt)
{
	this->hTreeItem = 0;
	this->itemObj = itemObj;
	this->parent = 0;
	this->txt = txt->Clone();
}

UI::GUITreeView::TreeItem::TreeItem(void *itemObj, Text::CString txt)
{
	this->hTreeItem = 0;
	this->itemObj = itemObj;
	this->parent = 0;
	this->txt = Text::String::New(txt);
}

UI::GUITreeView::TreeItem::~TreeItem()
{
	TreeItem *item;
	UOSInt i;
	i = this->children.GetCount();
	while (i-- > 0)
	{
		item = this->children.GetItem(i);
		DEL_CLASS(item);
	}
	this->txt->Release();
	if (this->hTreeItem)
	{
		MemFree(this->hTreeItem);
	}
}

void UI::GUITreeView::TreeItem::AddChild(UI::GUITreeView::TreeItem *child)
{
	this->children.Add(child);
	child->SetParent(this);
}
void UI::GUITreeView::TreeItem::SetParent(UI::GUITreeView::TreeItem *parent)
{
	this->parent = parent;
}

UI::GUITreeView::TreeItem *UI::GUITreeView::TreeItem::GetParent()
{
	return this->parent;
}

void *UI::GUITreeView::TreeItem::GetItemObj()
{
	return this->itemObj;
}

void UI::GUITreeView::TreeItem::SetHItem(void *hTreeItem)
{
	this->hTreeItem = hTreeItem;
}

void *UI::GUITreeView::TreeItem::GetHItem()
{
	return this->hTreeItem;
}

void UI::GUITreeView::TreeItem::SetText(Text::CString txt)
{
	if (txt.leng == 0)
	{
		return;
	}
	this->txt->Release();
	this->txt = Text::String::New(txt);
}

NotNullPtr<Text::String> UI::GUITreeView::TreeItem::GetText() const
{
	return this->txt;
}

UOSInt UI::GUITreeView::TreeItem::GetChildCount()
{
	return this->children.GetCount();
}

UI::GUITreeView::TreeItem *UI::GUITreeView::TreeItem::GetChild(UOSInt index)
{
	return this->children.GetItem(index);
}

void UI::GUITreeView::FreeItems()
{
	UI::GUITreeView::TreeItem *item;
	UOSInt i;
	i = this->treeItems.GetCount();
	while (i-- > 0)
	{
		item = this->treeItems.RemoveAt(i);
		DEL_CLASS(item);
	}
}

UI::GUITreeView::GUITreeView(GUICore *ui, UI::GUIClientControl *parent) : UI::GUIControl(ui, parent)
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->autoFocus = false;
	this->editing = false;
	this->draging = false;
	data->treeStore = gtk_tree_store_new(1, G_TYPE_STRING);
	data->treeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(data->treeStore));
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(data->treeView), false);

	GtkTreeViewColumn *col = gtk_tree_view_column_new();
//	gtk_tree_view_column_set_title(col, "Temp");
	gtk_tree_view_append_column(GTK_TREE_VIEW(data->treeView), col);

	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", 0);

	this->clsData = data;
	this->hwnd = (ControlHandle*)gtk_scrolled_window_new(0, 0);
	gtk_container_add(GTK_CONTAINER(this->hwnd), data->treeView);
	parent->AddChild(this);
	g_signal_connect(data->treeView, "cursor-changed", G_CALLBACK(GUITreeView_SelChange), this);
    g_signal_connect(data->treeView, "button-press-event", G_CALLBACK(GUITreeView_ButtonClick), this);
	gtk_widget_show(data->treeView);
	this->Show();
}

UI::GUITreeView::~GUITreeView()
{
	ClassData *data = this->clsData;
	FreeItems();
	MemFree(data);
}

void UI::GUITreeView::EventSelectionChange()
{
	UOSInt i = this->selChgHdlrs.GetCount();
	while (i-- > 0)
	{
		this->selChgHdlrs.GetItem(i)(this->selChgObjs.GetItem(i));
	}
}

void UI::GUITreeView::EventDoubleClick()
{
}

void UI::GUITreeView::EventRightClicked()
{
	UOSInt i = this->rightClkHdlrs.GetCount();
	while (i-- > 0)
	{
		this->rightClkHdlrs.GetItem(i)(this->rightClkObjs.GetItem(i));
	}
}

OSInt UI::GUITreeView::EventBeginLabelEdit(UI::GUITreeView::TreeItem *item)
{
	return 0;
}

OSInt UI::GUITreeView::EventEndLabelEdit(UI::GUITreeView::TreeItem *item, const UTF8Char *newLabel)
{
	return 0;
}

void UI::GUITreeView::EventDragItem(UI::GUITreeView::TreeItem *dragItem, TreeItem *dropItem)
{
}

UI::GUITreeView::TreeItem *UI::GUITreeView::InsertItem(TreeItem *parent, TreeItem *insertAfter, NotNullPtr<Text::String> itemText, void *itemObj)
{
	ClassData *data = this->clsData;
	TreeItem *item;
	GtkTreeIter *parentIter = 0;
	if (parent)
	{
		parentIter = (GtkTreeIter*)parent->GetHItem();
	}
	GtkTreeIter *iter = MemAlloc(GtkTreeIter, 1);
	if (insertAfter)
	{
		GtkTreeIter *siblingIter = (GtkTreeIter*)insertAfter->GetHItem();
		gtk_tree_store_insert_after(data->treeStore, iter, parentIter, siblingIter);
	}
	else
	{
		gtk_tree_store_append(data->treeStore, iter, parentIter);
	}
	gtk_tree_store_set(data->treeStore, iter, 0, itemText->v, -1);
	NEW_CLASS(item, TreeItem(itemObj, itemText));
	item->SetHItem(iter);

	if (parent)
	{
		parent->AddChild(item);
	}
	else
	{
		this->treeItems.Add(item);
	}
	return item;
}

UI::GUITreeView::TreeItem *UI::GUITreeView::InsertItem(UI::GUITreeView::TreeItem *parent, TreeItem *insertAfter, Text::CString itemText, void *itemObj)
{
	ClassData *data = this->clsData;
	TreeItem *item;
	GtkTreeIter *parentIter = 0;
	if (parent)
	{
		parentIter = (GtkTreeIter*)parent->GetHItem();
	}
	GtkTreeIter *iter = MemAlloc(GtkTreeIter, 1);
	if (insertAfter)
	{
		GtkTreeIter *siblingIter = (GtkTreeIter*)insertAfter->GetHItem();
		gtk_tree_store_insert_after(data->treeStore, iter, parentIter, siblingIter);
	}
	else
	{
		gtk_tree_store_append(data->treeStore, iter, parentIter);
	}
	gtk_tree_store_set(data->treeStore, iter, 0, itemText.v, -1);
	NEW_CLASS(item, TreeItem(itemObj, itemText));
	item->SetHItem(iter);

	if (parent)
	{
		parent->AddChild(item);
	}
	else
	{
		this->treeItems.Add(item);
	}
	return item;
}

void *UI::GUITreeView::RemoveItem(TreeItem *item)
{
	ClassData *data = this->clsData;
	UOSInt i = this->treeItems.IndexOf(item);
	if (i != INVALID_INDEX)
	{
		void *obj = item->GetItemObj();
		gtk_tree_store_remove(data->treeStore, (GtkTreeIter*)item->GetHItem());
		this->treeItems.RemoveAt(i);
		DEL_CLASS(item);
		return obj;
	}
	else
	{
		return 0;
	}
}

void UI::GUITreeView::ClearItems()
{
	ClassData *data = this->clsData;
	gtk_tree_store_clear(data->treeStore);
	FreeItems();
}

UOSInt UI::GUITreeView::GetRootCount()
{
	return this->treeItems.GetCount();
}

UI::GUITreeView::TreeItem *UI::GUITreeView::GetRootItem(UOSInt index)
{
	return this->treeItems.GetItem(index);
}

void UI::GUITreeView::ExpandItem(TreeItem *item)
{
	ClassData *data = this->clsData;
	GtkTreePath *path = gtk_tree_model_get_path((GtkTreeModel*)data->treeStore, (GtkTreeIter*)item->GetHItem());
	gtk_tree_view_expand_row((GtkTreeView*)data->treeView, path, false);
	gtk_tree_path_free(path);
}

Bool UI::GUITreeView::IsExpanded(TreeItem *item)
{
	ClassData *data = this->clsData;
	GtkTreePath *path = gtk_tree_model_get_path((GtkTreeModel*)data->treeStore, (GtkTreeIter*)item->GetHItem());
	Bool ret = gtk_tree_view_row_expanded((GtkTreeView*)data->treeView, path);
	gtk_tree_path_free(path);
	return ret;
}

void UI::GUITreeView::SetHasLines(Bool hasLines)
{
	ClassData *data = this->clsData;
	gtk_tree_view_set_enable_tree_lines((GtkTreeView*)data->treeView, hasLines);
}

void UI::GUITreeView::SetHasCheckBox(Bool hasCheckBox)
{
}

void UI::GUITreeView::SetHasButtons(Bool hasButtons)
{
}

void UI::GUITreeView::SetAutoFocus(Bool autoFocus)
{
}

UI::GUITreeView::TreeItem *GUITreeView_SearchChildSelected(UI::GUITreeView::ClassData *data, GtkTreePath *selPath, UI::GUITreeView::TreeItem *item)
{
	UOSInt i = item->GetChildCount();
	GtkTreePath *itemPath;
	UI::GUITreeView::TreeItem *child;
	while (i-- > 0)
	{
		child = item->GetChild(i);
		itemPath = gtk_tree_model_get_path((GtkTreeModel*)data->treeStore, (GtkTreeIter*)child->GetHItem());
		if (gtk_tree_path_compare(selPath, itemPath) == 0)
		{
			gtk_tree_path_free(itemPath);
			return child;
		}
		child = GUITreeView_SearchChildSelected(data, selPath, child);
		gtk_tree_path_free(itemPath);
		if (child)
		{
			return child;
		}
	}
	return 0;
}

UI::GUITreeView::TreeItem *UI::GUITreeView::GetSelectedItem()
{
	ClassData *data = this->clsData;
	GtkTreeSelection *sel = gtk_tree_view_get_selection((GtkTreeView*)data->treeView);
	GtkTreeIter iter;
	if (gtk_tree_selection_get_selected(sel, 0, &iter))
	{
		GtkTreePath *selPath = gtk_tree_model_get_path(GTK_TREE_MODEL(data->treeStore), &iter);
		GtkTreePath *itemPath;
		UOSInt i = 0;
		UOSInt j = this->treeItems.GetCount();
		UI::GUITreeView::TreeItem *item;
		while (i < j)
		{
			item = this->treeItems.GetItem(i);
			itemPath = gtk_tree_model_get_path((GtkTreeModel*)data->treeStore, (GtkTreeIter*)item->GetHItem());
			if (gtk_tree_path_compare(selPath, itemPath) == 0)
			{
				gtk_tree_path_free(itemPath);
				gtk_tree_path_free(selPath);
				return item;
			}
			item = GUITreeView_SearchChildSelected(data, selPath, item);
			gtk_tree_path_free(itemPath);
			if (item)
			{
				gtk_tree_path_free(selPath);
				return item;
			}
		}
		gtk_tree_path_free(selPath);
		return 0;
	}
	else
	{
		return 0;
	}
}

UI::GUITreeView::TreeItem *UI::GUITreeView::GetHighlightItem()
{
	return this->GetSelectedItem();
}

void UI::GUITreeView::BeginEdit(TreeItem *item)
{
}

Text::CString UI::GUITreeView::GetObjectClass()
{
	return CSTR("TreeView");
}

OSInt UI::GUITreeView::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUITreeView::HandleSelectionChange(UI::UIEvent hdlr, void *userObj)
{
	this->selChgHdlrs.Add(hdlr);
	this->selChgObjs.Add(userObj);
}

void UI::GUITreeView::HandleRightClick(UI::UIEvent hdlr, void *userObj)
{
	this->rightClkHdlrs.Add(hdlr);
	this->rightClkObjs.Add(userObj);
}

