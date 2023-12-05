#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GUICheckedListBox.h"
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

UI::GUICheckedListBox::GUICheckedListBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIListView(ui, parent, UI::GUIListView::LVSTYLE_SMALLICON, 1)
{
}

UI::GUICheckedListBox::~GUICheckedListBox()
{
}

Bool UI::GUICheckedListBox::GetItemChecked(UOSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return false;
	gboolean ret = false;
	gtk_tree_model_get((GtkTreeModel*)data->listStore, &r->iter, 1, &ret, -1);
	return ret;
}

void UI::GUICheckedListBox::SetItemChecked(UOSInt index, Bool isChecked)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return;
	gtk_list_store_set(data->listStore, &r->iter, 1, isChecked, -1);
}

Text::CStringNN UI::GUICheckedListBox::GetObjectClass() const
{
	return CSTR("CheckedListBox");
}

OSInt UI::GUICheckedListBox::OnNotify(UInt32 code, void *lParam)
{
	if (code == 0x1234)
	{
		GUIListViewData *data = (GUIListViewData*)this->clsData;
		GtkTreeIter iter;
		if (gtk_tree_model_get_iter_from_string((GtkTreeModel*)data->listStore, &iter, (gchar*)lParam))
		{
			gboolean ret = false;
			gtk_tree_model_get((GtkTreeModel*)data->listStore, &iter, 1, &ret, -1);
			ret = !ret;
			gtk_list_store_set(data->listStore, &iter, 1, ret, -1);
		}
	}
	return 0;
}
