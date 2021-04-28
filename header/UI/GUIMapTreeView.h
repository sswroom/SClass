#ifndef _SM_UI_GUIMAPTREEVIEW
#define _SM_UI_GUIMAPTREEVIEW
#include "UI/GUITreeView.h"
#include "Map/MapEnv.h"

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

		typedef void (__stdcall *TreeDragHandler)(void *userObj, ItemIndex *dragItem, ItemIndex *dropItem);

	private:
		Map::MapEnv *env;
		TreeDragHandler dragHdlr;
		void *dragObj;

	private:
		void RemoveItems();
		void FreeItem(UI::GUITreeView::TreeItem *item);
		void AddTreeNode(UI::GUITreeView::TreeItem *item, Map::MapEnv::GroupItem *group, OSInt index);
		void UpdateTreeStatus(UI::GUITreeView::TreeItem *item);
	public:
		GUIMapTreeView(UI::GUICore *ui, UI::GUIClientControl *parent, Map::MapEnv *env);
		virtual ~GUIMapTreeView();

		virtual OSInt EventBeginLabelEdit(TreeItem *item);
		virtual OSInt EventEndLabelEdit(TreeItem *item, const UTF8Char *newLabel);
		virtual void EventDragItem(TreeItem *dragItem, TreeItem *dropItem);
		virtual const UTF8Char *GetObjectClass();

		void SetDragHandler(TreeDragHandler dragHdlr, void *dragObj);
		void SetEnv(Map::MapEnv *env);
		void UpdateTree();
		void AddSubGroup(UI::GUITreeView::TreeItem *item);
		void RemoveItem(UI::GUITreeView::TreeItem *item);
		void ExpandColl(ItemIndex *ind);
	};
}
#endif
