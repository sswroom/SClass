#ifndef _SM_UI_GUICOMBOBOX
#define _SM_UI_GUICOMBOBOX
#include "Data/ArrayListStringNN.h"
#include "Text/String.h"
#include "UI/GUIClientControl.h"
#include "UI/ItemSelector.h"

namespace UI
{
	class GUIComboBox : public GUIControl, public ItemSelector
	{
	protected:
		Data::ArrayListObj<UI::UIEvent> selChgHdlrs;
		Data::ArrayListObj<AnyType> selChgObjs;
		Data::ArrayListStringNN itemTexts;
		Bool autoComplete;
		UIntOS lastTextLeng;
		Bool nonUIEvent;

	public:
		GUIComboBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUIComboBox();

		void EventSelectionChange();
		void EventTextChanged();

		virtual void SetText(Text::CStringNN text) = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetText(UnsafeArray<UTF8Char> buff) = 0;
		virtual Bool GetText(NN<Text::StringBuilderUTF8> sb) = 0;

		virtual void BeginUpdate() = 0;
		virtual void EndUpdate() = 0;
		virtual UIntOS AddItem(NN<Text::String> itemText, AnyType itemObj) = 0;
		virtual UIntOS AddItem(Text::CStringNN itemText, AnyType itemObj) = 0;
		virtual UIntOS InsertItem(UIntOS index, NN<Text::String> itemText, AnyType itemObj) = 0;
		virtual UIntOS InsertItem(UIntOS index, Text::CStringNN itemText, AnyType itemObj) = 0;
		virtual AnyType RemoveItem(UIntOS index) = 0;
		virtual void ClearItems() = 0;
		virtual UIntOS GetCount() = 0;
		virtual void SetSelectedIndex(UIntOS index) = 0;
		virtual UIntOS GetSelectedIndex() = 0;
		virtual AnyType GetSelectedItem() = 0;
		virtual AnyType GetItem(UIntOS index) = 0;

		UnsafeArrayOpt<UTF8Char> GetSelectedItemText(UnsafeArray<UTF8Char> buff);
		UnsafeArrayOpt<UTF8Char> GetItemText(UnsafeArray<UTF8Char> buff, UIntOS index);
		virtual Text::CStringNN GetObjectClass() const;

		virtual Math::Size2DDbl GetSize() = 0;
		virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn) = 0;
		virtual IntOS OnNotify(UInt32 code, void *lParam) = 0;
		virtual void UpdatePos(Bool redraw) = 0;
		virtual void SetTextSelection(UIntOS startPos, UIntOS endPos) = 0;

		virtual void HandleSelectionChange(UI::UIEvent hdlr, AnyType userObj);
		void SetAutoComplete(Bool autoComplete);
	};
}
#endif
