#ifndef _SM_UI_GUITREEVIEW
#define _SM_UI_GUITREEVIEW
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITreeView : public GUIControl
	{
	public:
		class TreeItem
		{
		private:
			Data::ArrayList<TreeItem *> *children;
			void *hTreeItem;
			void *itemObj;
			const UTF8Char *txt;
			TreeItem *parent;

		public:
			TreeItem(void *itemObj, const UTF8Char *txt);
			~TreeItem();

			void AddChild(TreeItem *child);
			void SetParent(TreeItem *parent);
			TreeItem *GetParent();
			void *GetItemObj();
			void SetHItem(void *hTreeItem);
			void *GetHItem();
			void SetText(const UTF8Char *txt);
			const UTF8Char *GetText();
			UOSInt GetChildCount();
			TreeItem *GetChild(UOSInt index);
		};
	private:
		Data::ArrayList<UI::UIEvent> *selChgHdlrs;
		Data::ArrayList<void *> *selChgObjs;
		Data::ArrayList<UI::UIEvent> *rightClkHdlrs;
		Data::ArrayList<void *> *rightClkObjs;
		Data::ArrayList<TreeItem*> *treeItems;
		Bool autoFocus;
		Bool editing;
		void *oriWndProc;
		Bool draging;
		void *himgDrag;
		TreeItem *dragItem;
		void *clsData;

		static OSInt __stdcall TVWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		void FreeItems();
	public:
		GUITreeView(GUICore *ui, UI::GUIClientControl *parent);
		virtual ~GUITreeView();

		void EventSelectionChange();
		void EventDoubleClick();
		void EventRightClicked();
		virtual OSInt EventBeginLabelEdit(TreeItem *item);
		virtual OSInt EventEndLabelEdit(TreeItem *item, const UTF8Char *newLabel);
		virtual void EventDragItem(TreeItem *dragItem, TreeItem *dropItem);

		TreeItem *InsertItem(TreeItem *parent, TreeItem *insertAfter, const UTF8Char *itemText, void *itemObj);
		void *RemoveItem(TreeItem *item);
		void ClearItems();
		UOSInt GetRootCount();
		TreeItem *GetRootItem(OSInt index);
		void ExpandItem(TreeItem *item);
		Bool IsExpanded(TreeItem *item);
		void SetHasLines(Bool hasLines);
		void SetHasCheckBox(Bool hasCheckBox);
		void SetHasButtons(Bool hasButtons);
		void SetAutoFocus(Bool autoFocus);
		UI::GUITreeView::TreeItem *GetSelectedItem();
		UI::GUITreeView::TreeItem *GetHighlightItem();
		void BeginEdit(TreeItem *item);

		virtual const UTF8Char *GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		virtual void HandleSelectionChange(UI::UIEvent hdlr, void *userObj);
		virtual void HandleRightClick(UI::UIEvent hdlr, void *userObj);
	};
}
#endif
