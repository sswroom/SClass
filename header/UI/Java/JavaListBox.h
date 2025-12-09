#ifndef _SM_UI_JAVA_JAVALISTBOX
#define _SM_UI_JAVA_JAVALISTBOX
#include "UI/GUIListBox.h"

namespace UI
{
	namespace Java
	{
		class JavaListBox : public UI::GUIListBox
		{
		public:
			JavaListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool multiSelect);
			virtual ~JavaListBox();

			virtual UOSInt AddItem(NN<Text::String> itemText, AnyType itemObj);
			virtual UOSInt AddItem(Text::CStringNN itemText, AnyType itemObj);
			virtual UOSInt InsertItem(UOSInt index, NN<Text::String> itemText, AnyType itemObj);
			virtual UOSInt InsertItem(UOSInt index, Text::CStringNN itemText, AnyType itemObj);
			virtual AnyType RemoveItem(UOSInt index);
			virtual AnyType GetItem(UOSInt index);
			virtual void ClearItems();
			virtual UOSInt GetCount();
			virtual void SetSelectedIndex(UOSInt index);
			virtual UOSInt GetSelectedIndex();
			virtual Bool GetSelectedIndices(NN<Data::ArrayList<UInt32>> indices);
			virtual AnyType GetSelectedItem();
			virtual UnsafeArrayOpt<UTF8Char> GetSelectedItemText(UnsafeArray<UTF8Char> buff);
			virtual Optional<Text::String> GetSelectedItemTextNew();
			virtual UnsafeArrayOpt<UTF8Char> GetItemText(UnsafeArray<UTF8Char> buff, UOSInt index);
			virtual void SetItemText(UOSInt index, Text::CStringNN text);
			virtual Optional<Text::String> GetItemTextNew(UOSInt index);
			virtual OSInt GetItemHeight();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
