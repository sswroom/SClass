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
			JavaListBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Bool multiSelect);
			virtual ~JavaListBox();

			virtual UOSInt AddItem(NotNullPtr<Text::String> itemText, void *itemObj);
			virtual UOSInt AddItem(Text::CStringNN itemText, void *itemObj);
			virtual UOSInt InsertItem(UOSInt index, Text::String *itemText, void *itemObj);
			virtual UOSInt InsertItem(UOSInt index, Text::CStringNN itemText, void *itemObj);
			virtual void *RemoveItem(UOSInt index);
			virtual void *GetItem(UOSInt index);
			virtual void ClearItems();
			virtual UOSInt GetCount();
			virtual void SetSelectedIndex(UOSInt index);
			virtual UOSInt GetSelectedIndex();
			virtual Bool GetSelectedIndices(Data::ArrayList<UInt32> *indices);
			virtual void *GetSelectedItem();
			virtual UTF8Char *GetSelectedItemText(UTF8Char *buff);
			virtual Optional<Text::String> GetSelectedItemTextNew();
			virtual UTF8Char *GetItemText(UTF8Char *buff, UOSInt index);
			virtual void SetItemText(UOSInt index, Text::CStringNN text);
			virtual Optional<Text::String> GetItemTextNew(UOSInt index);
			virtual OSInt GetItemHeight();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
