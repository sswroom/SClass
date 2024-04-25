#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GTK/GTKCheckedListBox.h"
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
	UI::ListViewStyle lvstyle;
	Bool noChgEvt;
} GUIListViewData;

UI::GTK::GTKCheckedListBox::GTKCheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUICheckedListBox(ui, parent)
{
}

UI::GTK::GTKCheckedListBox::~GTKCheckedListBox()
{
}

Bool UI::GTK::GTKCheckedListBox::GetItemChecked(UOSInt index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return false;
	gboolean ret = false;
	gtk_tree_model_get((GtkTreeModel*)data->listStore, &r->iter, 1, &ret, -1);
	return ret;
}

void UI::GTK::GTKCheckedListBox::SetItemChecked(UOSInt index, Bool isChecked)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	MyRow *r = data->rows->GetItem(index);
	if (r == 0)
		return;
	gtk_list_store_set(data->listStore, &r->iter, 1, isChecked, -1);
}

OSInt UI::GTK::GTKCheckedListBox::OnNotify(UInt32 code, void *lParam)
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
