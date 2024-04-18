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

UI::GUITreeView::TreeItem::TreeItem(AnyType itemObj, NotNullPtr<Text::String> txt)
{
	this->hTreeItem = 0;
	this->itemObj = itemObj;
	this->parent = 0;
	this->txt = txt->Clone();
}

UI::GUITreeView::TreeItem::TreeItem(AnyType itemObj, Text::CStringNN txt)
{
	this->hTreeItem = 0;
	this->itemObj = itemObj;
	this->parent = 0;
	this->txt = Text::String::New(txt);
}

UI::GUITreeView::TreeItem::~TreeItem()
{
	NotNullPtr<TreeItem> item;
	UOSInt i;
	i = this->children.GetCount();
	while (i-- > 0)
	{
		item = this->children.GetItemNoCheck(i);
		item.Delete();
	}
	this->txt->Release();
	if (this->hTreeItem)
	{
		MemFree(this->hTreeItem);
	}
}

void UI::GUITreeView::TreeItem::AddChild(NotNullPtr<UI::GUITreeView::TreeItem> child)
{
	this->children.Add(child);
	child->SetParent(this);
}
void UI::GUITreeView::TreeItem::SetParent(Optional<UI::GUITreeView::TreeItem> parent)
{
	this->parent = parent;
}

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::TreeItem::GetParent()
{
	return this->parent;
}

AnyType UI::GUITreeView::TreeItem::GetItemObj()
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

void UI::GUITreeView::TreeItem::SetText(Text::CStringNN txt)
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

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::TreeItem::GetChild(UOSInt index)
{
	return this->children.GetItem(index);
}

void UI::GUITreeView::FreeItems()
{
	this->treeItems.DeleteAll();
}

UI::GUITreeView::GUITreeView(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
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
	parent->AddChild(*this);
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

OSInt UI::GUITreeView::EventBeginLabelEdit(NotNullPtr<UI::GUITreeView::TreeItem> item)
{
	return 0;
}

OSInt UI::GUITreeView::EventEndLabelEdit(NotNullPtr<UI::GUITreeView::TreeItem> item, const UTF8Char *newLabel)
{
	return 0;
}

void UI::GUITreeView::EventDragItem(NotNullPtr<UI::GUITreeView::TreeItem> dragItem, NotNullPtr<TreeItem> dropItem)
{
}

NotNullPtr<UI::GUITreeView::TreeItem> UI::GUITreeView::InsertItem(Optional<TreeItem> parent, Optional<TreeItem> insertAfter, NotNullPtr<Text::String> itemText, AnyType itemObj)
{
	ClassData *data = this->clsData;
	NotNullPtr<TreeItem> item;
	NotNullPtr<TreeItem> nnparent;
	GtkTreeIter *parentIter = 0;
	if (parent.SetTo(nnparent))
	{
		parentIter = (GtkTreeIter*)nnparent->GetHItem();
	}
	GtkTreeIter *iter = MemAlloc(GtkTreeIter, 1);
	if (insertAfter.SetTo(item))
	{
		GtkTreeIter *siblingIter = (GtkTreeIter*)item->GetHItem();
		gtk_tree_store_insert_after(data->treeStore, iter, parentIter, siblingIter);
	}
	else
	{
		gtk_tree_store_append(data->treeStore, iter, parentIter);
	}
	gtk_tree_store_set(data->treeStore, iter, 0, itemText->v, -1);
	NEW_CLASSNN(item, TreeItem(itemObj, itemText));
	item->SetHItem(iter);

	if (parent.SetTo(nnparent))
	{
		nnparent->AddChild(item);
	}
	else
	{
		this->treeItems.Add(item);
	}
	return item;
}

NotNullPtr<UI::GUITreeView::TreeItem> UI::GUITreeView::InsertItem(Optional<UI::GUITreeView::TreeItem> parent, Optional<TreeItem> insertAfter, Text::CStringNN itemText, AnyType itemObj)
{
	ClassData *data = this->clsData;
	NotNullPtr<TreeItem> item;
	NotNullPtr<TreeItem> nnparent;
	GtkTreeIter *parentIter = 0;
	if (parent.SetTo(nnparent))
	{
		parentIter = (GtkTreeIter*)nnparent->GetHItem();
	}
	GtkTreeIter *iter = MemAlloc(GtkTreeIter, 1);
	if (insertAfter.SetTo(item))
	{
		GtkTreeIter *siblingIter = (GtkTreeIter*)item->GetHItem();
		gtk_tree_store_insert_after(data->treeStore, iter, parentIter, siblingIter);
	}
	else
	{
		gtk_tree_store_append(data->treeStore, iter, parentIter);
	}
	gtk_tree_store_set(data->treeStore, iter, 0, itemText.v, -1);
	NEW_CLASSNN(item, TreeItem(itemObj, itemText));
	item->SetHItem(iter);

	if (parent.SetTo(nnparent))
	{
		nnparent->AddChild(item);
	}
	else
	{
		this->treeItems.Add(item);
	}
	return item;
}

AnyType UI::GUITreeView::RemoveItem(NotNullPtr<TreeItem> item)
{
	ClassData *data = this->clsData;
	UOSInt i = this->treeItems.IndexOf(item);
	if (i != INVALID_INDEX)
	{
		AnyType obj = item->GetItemObj();
		gtk_tree_store_remove(data->treeStore, (GtkTreeIter*)item->GetHItem());
		this->treeItems.RemoveAt(i);
		item.Delete();
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

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::GetRootItem(UOSInt index)
{
	return this->treeItems.GetItem(index);
}

void UI::GUITreeView::ExpandItem(NotNullPtr<TreeItem> item)
{
	ClassData *data = this->clsData;
	GtkTreePath *path = gtk_tree_model_get_path((GtkTreeModel*)data->treeStore, (GtkTreeIter*)item->GetHItem());
	gtk_tree_view_expand_row((GtkTreeView*)data->treeView, path, false);
	gtk_tree_path_free(path);
}

Bool UI::GUITreeView::IsExpanded(NotNullPtr<TreeItem> item)
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

Optional<UI::GUITreeView::TreeItem> GUITreeView_SearchChildSelected(UI::GUITreeView::ClassData *data, GtkTreePath *selPath, NotNullPtr<UI::GUITreeView::TreeItem> item)
{
	UOSInt i = item->GetChildCount();
	GtkTreePath *itemPath;
	NotNullPtr<UI::GUITreeView::TreeItem> child;
	while (i-- > 0)
	{
		if (item->GetChild(i).SetTo(child))
		{
			itemPath = gtk_tree_model_get_path((GtkTreeModel*)data->treeStore, (GtkTreeIter*)child->GetHItem());
			if (gtk_tree_path_compare(selPath, itemPath) == 0)
			{
				gtk_tree_path_free(itemPath);
				return child;
			}
			if (GUITreeView_SearchChildSelected(data, selPath, child).SetTo(child))
			{
				gtk_tree_path_free(itemPath);
				return child;
			}
			gtk_tree_path_free(itemPath);
		}
	}
	return 0;
}

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::GetSelectedItem()
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
		NotNullPtr<UI::GUITreeView::TreeItem> item;
		while (i < j)
		{
			item = this->treeItems.GetItemNoCheck(i);
			itemPath = gtk_tree_model_get_path((GtkTreeModel*)data->treeStore, (GtkTreeIter*)item->GetHItem());
			if (gtk_tree_path_compare(selPath, itemPath) == 0)
			{
				gtk_tree_path_free(itemPath);
				gtk_tree_path_free(selPath);
				return item;
			}
			if (GUITreeView_SearchChildSelected(data, selPath, item).SetTo(item))
			{
				gtk_tree_path_free(itemPath);
				gtk_tree_path_free(selPath);
				return item;
			}
			gtk_tree_path_free(itemPath);
		}
		gtk_tree_path_free(selPath);
		return 0;
	}
	else
	{
		return 0;
	}
}

Optional<UI::GUITreeView::TreeItem> UI::GUITreeView::GetHighlightItem()
{
	return this->GetSelectedItem();
}

void UI::GUITreeView::BeginEdit(NotNullPtr<TreeItem> item)
{
}

Text::CStringNN UI::GUITreeView::GetObjectClass() const
{
	return CSTR("TreeView");
}

OSInt UI::GUITreeView::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::GUITreeView::HandleSelectionChange(UI::UIEvent hdlr, AnyType userObj)
{
	this->selChgHdlrs.Add(hdlr);
	this->selChgObjs.Add(userObj);
}

void UI::GUITreeView::HandleRightClick(UI::UIEvent hdlr, AnyType userObj)
{
	this->rightClkHdlrs.Add(hdlr);
	this->rightClkObjs.Add(userObj);
}

