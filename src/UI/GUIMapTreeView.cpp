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
	NN<TreeItem> item;
	if (this->GetRootCount() > 0 && this->GetRootItem(0).SetTo(item))
	{
		FreeItem(item);
		this->ClearItems();
	}
}

void UI::GUIMapTreeView::FreeItem(NN<UI::GUITreeView::TreeItem> item)
{
	NN<ItemIndex> ind = item->GetItemObj().GetNN<ItemIndex>();
	MemFreeNN(ind);
	NN<TreeItem> child;
	UOSInt i = item->GetChildCount();
	while (i-- > 0)
	{
		if (item->GetChild(i).SetTo(child))
			FreeItem(child);
	}
}

void UI::GUIMapTreeView::AddTreeNode(Optional<UI::GUITreeView::TreeItem> treeItem, Optional<Map::MapEnv::GroupItem> group, UOSInt index)
{
	NN<Map::MapEnv::MapItem> item;
	NN<ItemIndex> ind;
	if (this->env->GetItem(group, index).SetTo(item))
	{
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			NN<Map::MapEnv::LayerItem> layer = NN<Map::MapEnv::LayerItem>::ConvertFrom(item);
			NN<Text::String> name = layer->layer->GetName();
			UOSInt i = name->LastIndexOf(IO::Path::PATH_SEPERATOR);
			ind = MemAllocNN(ItemIndex);
			ind->group = group;
			ind->index = index;
			ind->itemType = Map::MapEnv::IT_LAYER;
			ind->item = item;
			treeItem = this->InsertItem(treeItem, 0, name->ToCString().Substring(i + 1), ind);
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			NN<Map::MapEnv::GroupItem> grp = NN<Map::MapEnv::GroupItem>::ConvertFrom(item);
			UOSInt i;
			UOSInt j = this->env->GetItemCount(grp);
			ind = MemAllocNN(ItemIndex);
			ind->group = group;
			ind->index = index;
			ind->itemType = Map::MapEnv::IT_GROUP;
			ind->item = item;
			NN<TreeItem> nntreeItem;
			if (this->InsertItem(treeItem, 0, this->env->GetGroupName(grp), ind).SetTo(nntreeItem))
			{
				i = 0;
				while (i < j)
				{
					AddTreeNode(nntreeItem, grp, i);
					i++;
				}
				if (env->GetGroupHide(grp))
				{
				}
				else
				{
					this->ExpandItem(nntreeItem);
				}
			}
		}
	}
}

void UI::GUIMapTreeView::UpdateTreeStatus(NN<UI::GUITreeView::TreeItem> item)
{
	NN<ItemIndex> ind = item->GetItemObj().GetNN<ItemIndex>();
	if (ind->itemType == Map::MapEnv::IT_GROUP)
	{
		NN<Map::MapEnv::MapItem> mitem;
		if (ind->item.SetTo(mitem))
		{
			env->SetGroupHide(NN<Map::MapEnv::GroupItem>::ConvertFrom(mitem), !this->IsExpanded(item));
		}
	}
	if (ind->itemType != Map::MapEnv::IT_UNKNOWN)
	{
		NN<TreeItem> child;
		UOSInt i = item->GetChildCount();
		while (i-- > 0)
		{
			if (item->GetChild(i).SetTo(child))
				UpdateTreeStatus(child);
		}
	}
}

UI::GUIMapTreeView::GUIMapTreeView(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Map::MapEnv> env) : UI::GUITreeView(ui, parent)
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

OSInt UI::GUIMapTreeView::EventBeginLabelEdit(NN<TreeItem> item)
{
	NN<ItemIndex> ind = item->GetItemObj().GetNN<ItemIndex>();
	if (ind->item.NotNull() && ind->itemType == Map::MapEnv::IT_GROUP)
		return 0;
	else
		return 1;
}

OSInt UI::GUIMapTreeView::EventEndLabelEdit(NN<TreeItem> item, const UTF8Char *newLabel)
{
	NN<ItemIndex> ind = item->GetItemObj().GetNN<ItemIndex>();
	if (newLabel == 0)
		return 0;
	NN<Map::MapEnv::MapItem> mitem;
	if (ind->item.SetTo(mitem) && ind->itemType == Map::MapEnv::IT_GROUP)
	{
		this->env->SetGroupName(NN<Map::MapEnv::GroupItem>::ConvertFrom(mitem), Text::CString::FromPtr(newLabel));
		return 1;
	}
	else
		return 0;
}

void UI::GUIMapTreeView::EventDragItem(NN<TreeItem> dragItem, NN<TreeItem> dropItem)
{
	if (this->dragHdlr.func)
	{
		this->dragHdlr.func(this->dragHdlr.userObj, dragItem->GetItemObj().GetNN<ItemIndex>(), dropItem->GetItemObj().GetNN<ItemIndex>());
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

void UI::GUIMapTreeView::SetEnv(NN<Map::MapEnv> env)
{
	this->env = env;
	UpdateTree();
}

void UI::GUIMapTreeView::UpdateTree()
{
	UOSInt i;
	UOSInt j;
	NN<ItemIndex> ind;
	NN<TreeItem> item;

	if (this->GetRootCount() > 0 && this->GetRootItem(0).SetTo(item))
	{
		UpdateTreeStatus(item);
	}
	this->RemoveItems();
	ind = MemAllocNN(ItemIndex);
	ind->group = 0;
	ind->index = (UOSInt)-1;
	ind->itemType = Map::MapEnv::IT_GROUP;
	ind->item = 0;
	if (this->InsertItem(0, 0, CSTR("ROOT"), ind).SetTo(item))
	{
		j = this->env->GetItemCount(0);
		i = 0;
		while (i < j)
		{
			this->AddTreeNode(item, 0, i);
			i++;
		}
		this->ExpandItem(item);
	}
}

void UI::GUIMapTreeView::AddSubGroup(NN<UI::GUITreeView::TreeItem> item)
{
	NN<UI::GUITreeView::TreeItem> n;
	NN<UI::GUIMapTreeView::ItemIndex> ind;
	ind = item->GetItemObj().GetNN<ItemIndex>();
	if (ind->itemType == Map::MapEnv::IT_GROUP)
	{
		NN<Map::MapEnv::GroupItem> grp = this->env->AddGroup(Optional<Map::MapEnv::GroupItem>::ConvertFrom(ind->item), CSTR("Group"));
		ind = MemAllocNN(ItemIndex);
		ind->itemType = Map::MapEnv::IT_GROUP;
		ind->group = Optional<Map::MapEnv::GroupItem>::ConvertFrom(item->GetItemObj().GetNN<ItemIndex>()->item);
		ind->item = grp;
		ind->index = this->env->GetItemCount(Optional<Map::MapEnv::GroupItem>::ConvertFrom(item->GetItemObj().GetNN<ItemIndex>()->item)) - 1;
		if (this->InsertItem(item, 0, CSTR("Group"), ind).SetTo(n))
		{
			this->ExpandItem(n);
			this->BeginEdit(n);
		}
	}
	else if (ind->itemType == Map::MapEnv::IT_LAYER)
	{
		NN<Map::MapEnv::GroupItem> grp = this->env->AddGroup(item->GetItemObj().GetNN<ItemIndex>()->group, CSTR("Group"));
		ind = MemAllocNN(ItemIndex);
		ind->itemType = Map::MapEnv::IT_GROUP;
		ind->group = item->GetItemObj().GetNN<ItemIndex>()->group;
		ind->item = grp;
		if (ind->group.IsNull())
		{
			ind->index = this->env->GetItemCount(0) - 1;
			if (this->InsertItem(0, 0, CSTR("Group"), ind).SetTo(n))
			{
				this->ExpandItem(n);
				this->BeginEdit(n);
			}
		}
		else
		{
			ind->index = this->env->GetItemCount(item->GetItemObj().GetNN<ItemIndex>()->group) - 1;
			if (this->InsertItem(item->GetParent(), 0, CSTR("Group"), ind).SetTo(n))
			{
				this->ExpandItem(n);
				this->BeginEdit(n);
			}
		}
	}
}

void UI::GUIMapTreeView::RemoveItem(NN<UI::GUITreeView::TreeItem> item)
{
	NN<ItemIndex> ind = item->GetItemObj().GetNN<ItemIndex>();
	item->GetChild(ind->index);
	this->env->RemoveItem(ind->group, ind->index);
	ind->itemType = Map::MapEnv::IT_UNKNOWN;
	this->UpdateTree();
}

void UI::GUIMapTreeView::ExpandColl(NN<UI::GUIMapTreeView::ItemIndex> ind)
{
	NN<Map::MapEnv::LayerItem> lyr;
	if (ind->itemType == Map::MapEnv::IT_LAYER && Optional<Map::MapEnv::LayerItem>::ConvertFrom(ind->item).SetTo(lyr))
	{
		if (lyr->layer->GetObjectClass() == Map::MapDrawLayer::OC_MAP_LAYER_COLL)
		{
			NN<Map::MapLayerCollection> lyrColl = NN<Map::MapLayerCollection>::ConvertFrom(lyr);
			NN<Map::MapEnv::GroupItem> grp;
			NN<Map::MapDrawLayer> layer;
			UOSInt i = this->env->GetItemCount(ind->group);
			grp = this->env->AddGroup(ind->group, lyrColl->GetName());
			Sync::RWMutexUsage mutUsage;
			Data::ArrayIterator<NN<Map::MapDrawLayer>> it = lyrColl->Iterator(mutUsage);
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
