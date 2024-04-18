#ifndef _SM_UI_GUITREEVIEW
#define _SM_UI_GUITREEVIEW
#include "Data/ArrayListNN.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUITreeView : public GUIControl
	{
	public:
		class TreeItem
		{
		private:
			Data::ArrayListNN<TreeItem> children;
			void *hTreeItem;
			AnyType itemObj;
			NotNullPtr<Text::String> txt;
			Optional<TreeItem> parent;

		public:
			TreeItem(AnyType itemObj, NotNullPtr<Text::String> txt);
			TreeItem(AnyType itemObj, Text::CStringNN txt);
			~TreeItem();

			void AddChild(NotNullPtr<TreeItem> child);
			void SetParent(Optional<TreeItem> parent);
			Optional<TreeItem> GetParent();
			AnyType GetItemObj();
			void SetHItem(void *hTreeItem);
			void *GetHItem();
			void SetText(Text::CStringNN txt);
			NotNullPtr<Text::String> GetText() const;
			UOSInt GetChildCount();
			Optional<TreeItem> GetChild(UOSInt index);
		};

		struct ClassData;

	private:
		Data::ArrayList<UI::UIEvent> selChgHdlrs;
		Data::ArrayList<AnyType> selChgObjs;
		Data::ArrayList<UI::UIEvent> rightClkHdlrs;
		Data::ArrayList<AnyType> rightClkObjs;
		Data::ArrayListNN<TreeItem> treeItems;
		Bool autoFocus;
		Bool editing;
		void *oriWndProc;
		Bool draging;
		void *himgDrag;
		TreeItem *dragItem;
		ClassData *clsData;

		static OSInt __stdcall TVWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		void FreeItems();
	public:
		GUITreeView(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUITreeView();

		void EventSelectionChange();
		void EventDoubleClick();
		void EventRightClicked();
		virtual OSInt EventBeginLabelEdit(NotNullPtr<TreeItem> item);
		virtual OSInt EventEndLabelEdit(NotNullPtr<TreeItem> item, const UTF8Char *newLabel);
		virtual void EventDragItem(NotNullPtr<TreeItem> dragItem, NotNullPtr<TreeItem> dropItem);

		Optional<TreeItem> InsertItem(Optional<TreeItem> parent, Optional<TreeItem> insertAfter, NotNullPtr<Text::String> itemText, AnyType itemObj);
		Optional<TreeItem> InsertItem(Optional<TreeItem> parent, Optional<TreeItem> insertAfter, Text::CStringNN itemText, AnyType itemObj);
		AnyType RemoveItem(NotNullPtr<TreeItem> item);
		void ClearItems();
		UOSInt GetRootCount();
		Optional<TreeItem> GetRootItem(UOSInt index);
		void ExpandItem(NotNullPtr<TreeItem> item);
		Bool IsExpanded(NotNullPtr<TreeItem> item);
		void SetHasLines(Bool hasLines);
		void SetHasCheckBox(Bool hasCheckBox);
		void SetHasButtons(Bool hasButtons);
		void SetAutoFocus(Bool autoFocus);
		Optional<UI::GUITreeView::TreeItem> GetSelectedItem();
		Optional<UI::GUITreeView::TreeItem> GetHighlightItem();
		void BeginEdit(NotNullPtr<TreeItem> item);

		virtual Text::CStringNN GetObjectClass() const;
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		virtual void HandleSelectionChange(UI::UIEvent hdlr, AnyType userObj);
		virtual void HandleRightClick(UI::UIEvent hdlr, AnyType userObj);
	};
}
#endif
