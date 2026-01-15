#ifndef _SM_UI_WIN_WINCOMBOBOX
#define _SM_UI_WIN_WINCOMBOBOX
#include "UI/GUIComboBox.h"

namespace UI
{
	namespace Win
	{
		class WinComboBox : public GUIComboBox
		{
		private:
			UIntOS minVisible;
			Bool allowEdit;

		public:
			WinComboBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool allowEditing);
			virtual ~WinComboBox();

			virtual void SetText(Text::CStringNN text);
			virtual UnsafeArrayOpt<UTF8Char> GetText(UnsafeArray<UTF8Char> buff);
			virtual Bool GetText(NN<Text::StringBuilderUTF8> sb);

			virtual void BeginUpdate();
			virtual void EndUpdate();
			virtual UIntOS AddItem(NN<Text::String> itemText, AnyType itemObj);
			virtual UIntOS AddItem(Text::CStringNN itemText, AnyType itemObj);
			virtual UIntOS InsertItem(UIntOS index, NN<Text::String> itemText, AnyType itemObj);
			virtual UIntOS InsertItem(UIntOS index, Text::CStringNN itemText, AnyType itemObj);
			virtual AnyType RemoveItem(UIntOS index);
			virtual void ClearItems();
			virtual UIntOS GetCount();
			virtual void SetSelectedIndex(UIntOS index);
			virtual UIntOS GetSelectedIndex();
			virtual AnyType GetSelectedItem();
			virtual AnyType GetItem(UIntOS index);

			virtual Math::Size2DDbl GetSize();
			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
			virtual void SetTextSelection(UIntOS startPos, UIntOS endPos);

			UIntOS GetListMinVisible();
			Bool SetListMinVisible(UIntOS itemCount);
			UIntOS GetSelectionHeight();
			Bool SetListItemHeight(UIntOS itemHeight);
		};

	}
}
#endif
