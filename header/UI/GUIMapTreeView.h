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

		typedef void (__stdcall *TreeDragHandler)(AnyType userObj, NotNullPtr<ItemIndex> dragItem, NotNullPtr<ItemIndex> dropItem);

	private:
		NotNullPtr<Map::MapEnv> env;
		Data::CallbackStorage<TreeDragHandler> dragHdlr;

	private:
		void RemoveItems();
		void FreeItem(NotNullPtr<UI::GUITreeView::TreeItem> item);
		void AddTreeNode(Optional<UI::GUITreeView::TreeItem> item, Optional<Map::MapEnv::GroupItem> group, UOSInt index);
		void UpdateTreeStatus(NotNullPtr<UI::GUITreeView::TreeItem> item);
	public:
		GUIMapTreeView(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Map::MapEnv> env);
		virtual ~GUIMapTreeView();

		virtual OSInt EventBeginLabelEdit(NotNullPtr<TreeItem> item);
		virtual OSInt EventEndLabelEdit(NotNullPtr<TreeItem> item, const UTF8Char *newLabel);
		virtual void EventDragItem(NotNullPtr<TreeItem> dragItem, NotNullPtr<TreeItem> dropItem);
		virtual Text::CStringNN GetObjectClass() const;

		void SetDragHandler(TreeDragHandler dragHdlr, AnyType dragObj);
		void SetEnv(NotNullPtr<Map::MapEnv> env);
		void UpdateTree();
		void AddSubGroup(NotNullPtr<UI::GUITreeView::TreeItem> item);
		void RemoveItem(NotNullPtr<UI::GUITreeView::TreeItem> item);
		void ExpandColl(NotNullPtr<ItemIndex> ind);
	};
}
#endif
