#ifndef _SM_UI_GUIMAPTREEVIEW
#define _SM_UI_GUIMAPTREEVIEW
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Map/MapEnv.h"
#include "UI/GUITreeView.h"

namespace UI
{
	class GUIMapTreeView : public GUITreeView
	{
	public:
		typedef struct
		{
			Optional<Map::MapEnv::GroupItem> group;
			UOSInt index;
			Map::MapEnv::ItemType itemType;
			Optional<Map::MapEnv::MapItem> item;
		} ItemIndex;

		typedef void (__stdcall *TreeDragHandler)(AnyType userObj, NN<ItemIndex> dragItem, NN<ItemIndex> dropItem);

	private:
		NN<Map::MapEnv> env;
		Data::CallbackStorage<TreeDragHandler> dragHdlr;

	private:
		void RemoveItems();
		void FreeItem(NN<UI::GUITreeView::TreeItem> item);
		void AddTreeNode(Optional<UI::GUITreeView::TreeItem> item, Optional<Map::MapEnv::GroupItem> group, UOSInt index);
		void UpdateTreeStatus(NN<UI::GUITreeView::TreeItem> item);
	public:
		GUIMapTreeView(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Map::MapEnv> env);
		virtual ~GUIMapTreeView();

		virtual OSInt EventBeginLabelEdit(NN<TreeItem> item);
		virtual OSInt EventEndLabelEdit(NN<TreeItem> item, UnsafeArray<const UTF8Char> newLabel);
		virtual void EventDragItem(NN<TreeItem> dragItem, NN<TreeItem> dropItem);
		virtual Text::CStringNN GetObjectClass() const;

		void SetDragHandler(TreeDragHandler dragHdlr, AnyType dragObj);
		void SetEnv(NN<Map::MapEnv> env);
		void UpdateTree();
		void AddSubGroup(NN<UI::GUITreeView::TreeItem> item);
		void RemoveItem(NN<UI::GUITreeView::TreeItem> item);
		void ExpandColl(NN<ItemIndex> ind);
	};
}
#endif
