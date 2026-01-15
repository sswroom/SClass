#ifndef _SM_UI_JAVAUI_JUILISTBOX
#define _SM_UI_JAVAUI_JUILISTBOX
#include "UI/GUIListBox.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIListBox : public UI::GUIListBox
		{
		public:
			JUIListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool multiSelect);
			virtual ~JUIListBox();

			virtual UIntOS AddItem(NN<Text::String> itemText, AnyType itemObj);
			virtual UIntOS AddItem(Text::CStringNN itemText, AnyType itemObj);
			virtual UIntOS InsertItem(UIntOS index, NN<Text::String> itemText, AnyType itemObj);
			virtual UIntOS InsertItem(UIntOS index, Text::CStringNN itemText, AnyType itemObj);
			virtual AnyType RemoveItem(UIntOS index);
			virtual AnyType GetItem(UIntOS index);
			virtual void ClearItems();
			virtual UIntOS GetCount();
			virtual void SetSelectedIndex(UIntOS index);
			virtual UIntOS GetSelectedIndex();
			virtual Bool GetSelectedIndices(NN<Data::ArrayList<UInt32>> indices);
			virtual AnyType GetSelectedItem();
			virtual UnsafeArrayOpt<UTF8Char> GetSelectedItemText(UnsafeArray<UTF8Char> buff);
			virtual Optional<Text::String> GetSelectedItemTextNew();
			virtual UnsafeArrayOpt<UTF8Char> GetItemText(UnsafeArray<UTF8Char> buff, UIntOS index);
			virtual void SetItemText(UIntOS index, Text::CStringNN text);
			virtual Optional<Text::String> GetItemTextNew(UIntOS index);
			virtual IntOS GetItemHeight();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
