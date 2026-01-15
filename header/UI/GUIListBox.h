#ifndef _SM_UI_GUILISTBOX
#define _SM_UI_GUILISTBOX
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Text/CString.h"
#include "Text/String.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIListBox : public GUIControl
	{
	private:
		Data::ArrayListObj<Data::CallbackStorage<UI::UIEvent>> selChgHdlrs;
		Data::ArrayListObj<Data::CallbackStorage<UI::UIEvent>> dblClickHdlrs;
		Data::ArrayListObj<Data::CallbackStorage<UI::GUIControl::MouseEventHandler>> rightClickHdlrs;

	public:
		GUIListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUIListBox();

		virtual Text::CStringNN GetObjectClass() const;

		void EventSelectionChange();
		void EventDoubleClick();
		void EventRightClick(Math::Coord2D<IntOS> pos);
		virtual void HandleSelectionChange(UI::UIEvent hdlr, AnyType userObj);
		virtual void HandleDoubleClicked(UI::UIEvent hdlr, AnyType userObj);
		virtual void HandleRightClicked(UI::GUIControl::MouseEventHandler hdlr, AnyType userObj);

		virtual UIntOS AddItem(NN<Text::String> itemText, AnyType itemObj) = 0;
		virtual UIntOS AddItem(Text::CStringNN itemText, AnyType itemObj) = 0;
		virtual UIntOS InsertItem(UIntOS index, NN<Text::String> itemText, AnyType itemObj) = 0;
		virtual UIntOS InsertItem(UIntOS index, Text::CStringNN itemText, AnyType itemObj) = 0;
		virtual AnyType RemoveItem(UIntOS index) = 0;
		virtual AnyType GetItem(UIntOS index) = 0;
		virtual void ClearItems() = 0;
		virtual UIntOS GetCount() = 0;
		virtual void SetSelectedIndex(UIntOS index) = 0;
		virtual UIntOS GetSelectedIndex() = 0;
		virtual Bool GetSelectedIndices(NN<Data::ArrayListNative<UInt32>> indices) = 0;
		virtual AnyType GetSelectedItem() = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetSelectedItemText(UnsafeArray<UTF8Char> buff) = 0;
		virtual Optional<Text::String> GetSelectedItemTextNew() = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetItemText(UnsafeArray<UTF8Char> buff, UIntOS index) = 0;
		virtual void SetItemText(UIntOS index, Text::CStringNN text) = 0;
		virtual Optional<Text::String> GetItemTextNew(UIntOS index) = 0;
		virtual IntOS GetItemHeight() = 0;

		virtual IntOS OnNotify(UInt32 code, void *lParam) = 0;
	};
}
#endif
