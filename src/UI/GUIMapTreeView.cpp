#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIMapTreeView.h"

#define COLOR_GROUP 0xff98fb98
#define COLOR_LAYER 0xffeee8aa
#define COLOR_ROOT 0xffffb6c1

void UI::GUIMapTreeView::RemoveItems()
{
	if (this->GetRootCount() > 0)
	{
		FreeItem(this->GetRootItem(0));
		this->ClearItems();
	}
}

void UI::GUIMapTreeView::FreeItem(UI::GUITreeView::TreeItem *item)
{
	ItemIndex *ind = (ItemIndex*)item->GetItemObj();
	MemFree(ind);
	UOSInt i = item->GetChildCount();
	while (i-- > 0)
	{
		FreeItem(item->GetChild(i));
	}
}

void UI::GUIMapTreeView::AddTreeNode(UI::GUITreeView::TreeItem *treeItem, Map::MapEnv::GroupItem *group, UOSInt index)
{
	Map::MapEnv::MapItem *item;

	item = this->env->GetItem(group, index);

	ItemIndex *ind;

	if (item)
	{
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			Map::MapEnv::LayerItem *layer = (Map::MapEnv::LayerItem*)item;
			NotNullPtr<Text::String> name = layer->layer->GetName();
			UOSInt i = name->LastIndexOf(IO::Path::PATH_SEPERATOR);
			ind = MemAlloc(ItemIndex, 1);
			ind->group = group;
			ind->index = index;
			ind->itemType = Map::MapEnv::IT_LAYER;
			ind->item = item;
			treeItem = this->InsertItem(treeItem, 0, name->ToCString().Substring(i + 1), ind);
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			Map::MapEnv::GroupItem *grp = (Map::MapEnv::GroupItem*)item;
			UOSInt i;
			UOSInt j = this->env->GetItemCount(grp);
			ind = MemAlloc(ItemIndex, 1);
			ind->group = group;
			ind->index = index;
			ind->itemType = Map::MapEnv::IT_GROUP;
			ind->item = item;
			treeItem = this->InsertItem(treeItem, 0, this->env->GetGroupName(grp), ind);
			i = 0;
			while (i < j)
			{
				AddTreeNode(treeItem, grp, i);
				i++;
			}
			if (env->GetGroupHide(grp))
			{
			}
			else
			{
				this->ExpandItem(treeItem);
			}
		}
	}
}

void UI::GUIMapTreeView::UpdateTreeStatus(UI::GUITreeView::TreeItem *item)
{
	ItemIndex *ind = (ItemIndex *)item->GetItemObj();
	if (ind->itemType == Map::MapEnv::IT_GROUP)
	{
		if (ind->item != 0)
		{
			env->SetGroupHide((Map::MapEnv::GroupItem*)ind->item, !this->IsExpanded(item));
		}
	}
	if (ind->itemType != Map::MapEnv::IT_UNKNOWN)
	{
		UOSInt i = item->GetChildCount();
		while (i-- > 0)
		{
			UpdateTreeStatus(item->GetChild(i));
		}
	}
}

UI::GUIMapTreeView::GUIMapTreeView(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Map::MapEnv> env) : UI::GUITreeView(ui, parent)
{
	this->dragHdlr = 0;
	this->env = env;
	this->SetHasLines(true);
	this->SetHasButtons(true);
	//this->SetAutoFocus(true);
	UpdateTree();
}

UI::GUIMapTreeView::~GUIMapTreeView()
{
	RemoveItems();
}

OSInt UI::GUIMapTreeView::EventBeginLabelEdit(TreeItem *item)
{
	ItemIndex *ind = (ItemIndex*)item->GetItemObj();
	if (ind->item != 0 && ind->itemType == Map::MapEnv::IT_GROUP)
		return 0;
	else
		return 1;
}

OSInt UI::GUIMapTreeView::EventEndLabelEdit(TreeItem *item, const UTF8Char *newLabel)
{
	ItemIndex *ind = (ItemIndex*)item->GetItemObj();
	if (newLabel == 0)
		return 0;
	if (ind->item != 0 && ind->itemType == Map::MapEnv::IT_GROUP)
	{
		this->env->SetGroupName((Map::MapEnv::GroupItem*)ind->item, Text::CString::FromPtr(newLabel));
		return 1;
	}
	else
		return 0;
}

void UI::GUIMapTreeView::EventDragItem(TreeItem *dragItem, TreeItem *dropItem)
{
	if (this->dragHdlr.func)
	{
		this->dragHdlr.func(this->dragHdlr.userObj, (UI::GUIMapTreeView::ItemIndex*)dragItem->GetItemObj(), (UI::GUIMapTreeView::ItemIndex*)dropItem->GetItemObj());
	}
}

Text::CStringNN UI::GUIMapTreeView::GetObjectClass() const
{
	return CSTR("MapTreeView");
}

void UI::GUIMapTreeView::SetDragHandler(TreeDragHandler dragHdlr, AnyType dragObj)
{
	this->dragHdlr = {dragHdlr, dragObj};
}

void UI::GUIMapTreeView::SetEnv(NotNullPtr<Map::MapEnv> env)
{
	this->env = env;
	UpdateTree();
}

void UI::GUIMapTreeView::UpdateTree()
{
	UOSInt i;
	UOSInt j;
	ItemIndex *ind;

	if (this->GetRootCount() > 0)
	{
		UpdateTreeStatus(this->GetRootItem(0));
	}
	this->RemoveItems();
	ind = MemAlloc(ItemIndex, 1);
	ind->group = 0;
	ind->index = (UOSInt)-1;
	ind->itemType = Map::MapEnv::IT_GROUP;
	ind->item = 0;
	UI::GUIMapTreeView::TreeItem *item = this->InsertItem(0, 0, CSTR("ROOT"), ind);

	j = this->env->GetItemCount(0);
	i = 0;
	while (i < j)
	{
		this->AddTreeNode(item, 0, i);
		i++;
	}
	this->ExpandItem(item);
}

void UI::GUIMapTreeView::AddSubGroup(UI::GUITreeView::TreeItem *item)
{
	UI::GUITreeView::TreeItem *n;
	UI::GUIMapTreeView::ItemIndex *ind;
	ind = (UI::GUIMapTreeView::ItemIndex*)item->GetItemObj();
	if (ind->itemType == Map::MapEnv::IT_GROUP)
	{
		Map::MapEnv::GroupItem *grp = this->env->AddGroup((Map::MapEnv::GroupItem*)ind->item, CSTR("Group"));
		ind = MemAlloc(ItemIndex, 1);
		ind->itemType = Map::MapEnv::IT_GROUP;
		ind->group = (Map::MapEnv::GroupItem*)(((ItemIndex*)item->GetItemObj())->item);
		ind->item = grp;
		ind->index = this->env->GetItemCount((Map::MapEnv::GroupItem*) ((UI::GUIMapTreeView::ItemIndex*)item->GetItemObj())->item) - 1;
		n = this->InsertItem(item, 0, CSTR("Group"), ind);
		this->ExpandItem(n);
		this->BeginEdit(n);
	}
	else if (ind->itemType == Map::MapEnv::IT_LAYER)
	{
		Map::MapEnv::GroupItem *grp = this->env->AddGroup(((UI::GUIMapTreeView::ItemIndex*)item->GetItemObj())->group, CSTR("Group"));
		ind = MemAlloc(ItemIndex, 1);
		ind->itemType = Map::MapEnv::IT_GROUP;
		ind->group = ((ItemIndex*)item->GetItemObj())->group;
		ind->item = grp;
		if (ind->group == 0)
		{
			ind->index = this->env->GetItemCount(0) - 1;
			n = this->InsertItem(0, 0, CSTR("Group"), ind);
			this->ExpandItem(n);
			this->BeginEdit(n);
		}
		else
		{
			ind->index = this->env->GetItemCount(((ItemIndex*)item->GetItemObj())->group) - 1;
			n = this->InsertItem(item->GetParent(), 0, CSTR("Group"), ind);
			this->ExpandItem(n);
			this->BeginEdit(n);
		}
	}
}

void UI::GUIMapTreeView::RemoveItem(UI::GUITreeView::TreeItem *item)
{
	ItemIndex *ind = (ItemIndex*)item->GetItemObj();
	item->GetChild(ind->index);
	this->env->RemoveItem(ind->group, ind->index);
	ind->itemType = Map::MapEnv::IT_UNKNOWN;
	this->UpdateTree();
}

void UI::GUIMapTreeView::ExpandColl(UI::GUIMapTreeView::ItemIndex *ind)
{
	if (ind->itemType == Map::MapEnv::IT_LAYER)
	{
		Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)ind->item;
		if (lyr->layer->GetObjectClass() == Map::MapDrawLayer::OC_MAP_LAYER_COLL)
		{
			Map::MapLayerCollection *lyrColl = (Map::MapLayerCollection*)lyr;
			Map::MapEnv::GroupItem *grp;
			NotNullPtr<Map::MapDrawLayer> layer;
			UOSInt i = this->env->GetItemCount(ind->group);
			grp = this->env->AddGroup(ind->group, lyrColl->GetName());
			Sync::RWMutexUsage mutUsage;
			Data::ArrayIterator<NotNullPtr<Map::MapDrawLayer>> it = lyrColl->Iterator(mutUsage);
			while (it.HasNext())
			{
				layer = it.Next();
				this->env->AddLayer(grp, layer, true);
			}
			mutUsage.EndUse();
			lyrColl->Clear();
			this->env->RemoveItem(ind->group, ind->index);
			this->env->MoveItem(ind->group, i - 1, ind->group, ind->index);
			this->UpdateTree();
		}
	}
}
