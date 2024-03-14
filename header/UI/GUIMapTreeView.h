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
			Map::MapEnv::GroupItem *group;
			UOSInt index;
			Map::MapEnv::ItemType itemType;
			Map::MapEnv::MapItem *item;
		} ItemIndex;

		typedef void (__stdcall *TreeDragHandler)(AnyType userObj, ItemIndex *dragItem, ItemIndex *dropItem);

	private:
		NotNullPtr<Map::MapEnv> env;
		Data::CallbackStorage<TreeDragHandler> dragHdlr;

	private:
		void RemoveItems();
		void FreeItem(UI::GUITreeView::TreeItem *item);
		void AddTreeNode(UI::GUITreeView::TreeItem *item, Map::MapEnv::GroupItem *group, UOSInt index);
		void UpdateTreeStatus(UI::GUITreeView::TreeItem *item);
	public:
		GUIMapTreeView(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, NotNullPtr<Map::MapEnv> env);
		virtual ~GUIMapTreeView();

		virtual OSInt EventBeginLabelEdit(TreeItem *item);
		virtual OSInt EventEndLabelEdit(TreeItem *item, const UTF8Char *newLabel);
		virtual void EventDragItem(TreeItem *dragItem, TreeItem *dropItem);
		virtual Text::CStringNN GetObjectClass() const;

		void SetDragHandler(TreeDragHandler dragHdlr, AnyType dragObj);
		void SetEnv(NotNullPtr<Map::MapEnv> env);
		void UpdateTree();
		void AddSubGroup(UI::GUITreeView::TreeItem *item);
		void RemoveItem(UI::GUITreeView::TreeItem *item);
		void ExpandColl(ItemIndex *ind);
	};
}
#endif
