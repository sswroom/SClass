#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/GTK/GTKCheckedListBox.h"
#include <gtk/gtk.h>

typedef struct
{
	GtkTreeIter iter;
	AnyType data;
	NN<Text::String> txt;
} MyRow;

typedef struct
{
	GtkListStore *listStore;
	GtkWidget *treeView;
	UIntOS colCnt;
	Double *colSizes;
	Data::ArrayListNN<MyRow> *rows;
	UI::ListViewStyle lvstyle;
	Bool noChgEvt;
} GUIListViewData;

UI::GTK::GTKCheckedListBox::GTKCheckedListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUICheckedListBox(ui, parent)
{
}

UI::GTK::GTKCheckedListBox::~GTKCheckedListBox()
{
}

Bool UI::GTK::GTKCheckedListBox::GetItemChecked(UIntOS index)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	NN<MyRow> r;
	if (!data->rows->GetItem(index).SetTo(r))
		return false;
	gboolean ret = false;
	gtk_tree_model_get((GtkTreeModel*)data->listStore, &r->iter, 1, &ret, -1);
	return ret;
}

void UI::GTK::GTKCheckedListBox::SetItemChecked(UIntOS index, Bool isChecked)
{
	GUIListViewData *data = (GUIListViewData*)this->clsData;
	NN<MyRow> r;
	if (!data->rows->GetItem(index).SetTo(r))
		return;
	gtk_list_store_set(data->listStore, &r->iter, 1, isChecked, -1);
}

IntOS UI::GTK::GTKCheckedListBox::OnNotify(UInt32 code, IntOS lParam)
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
