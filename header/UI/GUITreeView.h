#ifndef _SM_UI_GUITREEVIEW
#define _SM_UI_GUITREEVIEW
#include "Data/ArrayListNN.hpp"
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
			NN<Text::String> txt;
			Optional<TreeItem> parent;

		public:
			TreeItem(AnyType itemObj, NN<Text::String> txt);
			TreeItem(AnyType itemObj, Text::CStringNN txt);
			~TreeItem();

			void AddChild(NN<TreeItem> child);
			void SetParent(Optional<TreeItem> parent);
			Optional<TreeItem> GetParent();
			AnyType GetItemObj();
			void SetHItem(void *hTreeItem);
			void *GetHItem();
			void SetText(Text::CStringNN txt);
			NN<Text::String> GetText() const;
			UIntOS GetChildCount();
			Optional<TreeItem> GetChild(UIntOS index);
		};

		struct ClassData;

	private:
		Data::ArrayListObj<UI::UIEvent> selChgHdlrs;
		Data::ArrayListObj<AnyType> selChgObjs;
		Data::ArrayListObj<UI::UIEvent> rightClkHdlrs;
		Data::ArrayListObj<AnyType> rightClkObjs;
		Data::ArrayListNN<TreeItem> treeItems;
		Bool autoFocus;
		Bool editing;
		void *oriWndProc;
		Bool draging;
		void *himgDrag;
		TreeItem *dragItem;
		ClassData *clsData;

		static IntOS __stdcall TVWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam);
		void FreeItems();
	public:
		GUITreeView(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUITreeView();

		void EventSelectionChange();
		void EventDoubleClick();
		void EventRightClicked();
		virtual IntOS EventBeginLabelEdit(NN<TreeItem> item);
		virtual IntOS EventEndLabelEdit(NN<TreeItem> item, UnsafeArray<const UTF8Char> newLabel);
		virtual void EventDragItem(NN<TreeItem> dragItem, NN<TreeItem> dropItem);

		Optional<TreeItem> InsertItem(Optional<TreeItem> parent, Optional<TreeItem> insertAfter, NN<Text::String> itemText, AnyType itemObj);
		Optional<TreeItem> InsertItem(Optional<TreeItem> parent, Optional<TreeItem> insertAfter, Text::CStringNN itemText, AnyType itemObj);
		AnyType RemoveItem(NN<TreeItem> item);
		void ClearItems();
		UIntOS GetRootCount();
		Optional<TreeItem> GetRootItem(UIntOS index);
		void ExpandItem(NN<TreeItem> item);
		Bool IsExpanded(NN<TreeItem> item);
		void SetHasLines(Bool hasLines);
		void SetHasCheckBox(Bool hasCheckBox);
		void SetHasButtons(Bool hasButtons);
		void SetAutoFocus(Bool autoFocus);
		Optional<UI::GUITreeView::TreeItem> GetSelectedItem();
		Optional<UI::GUITreeView::TreeItem> GetHighlightItem();
		void BeginEdit(NN<TreeItem> item);

		virtual Text::CStringNN GetObjectClass() const;
		virtual IntOS OnNotify(UInt32 code, void *lParam);

		virtual void HandleSelectionChange(UI::UIEvent hdlr, AnyType userObj);
		virtual void HandleRightClick(UI::UIEvent hdlr, AnyType userObj);
	};
}
#endif
