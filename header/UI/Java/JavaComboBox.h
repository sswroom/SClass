#ifndef _SM_UI_JAVA_JAVACOMBOBOX
#define _SM_UI_JAVA_JAVACOMBOBOX
#include "UI/GUIComboBox.h"

namespace UI
{
	namespace Java
	{
		class JavaComboBox : public GUIComboBox
		{
		private:
			UOSInt minVisible;
			Bool allowEdit;

		public:
			JavaComboBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Bool allowEditing);
			virtual ~JavaComboBox();

			virtual void SetText(Text::CStringNN text);
			virtual UTF8Char *GetText(UTF8Char *buff);
			virtual Bool GetText(NotNullPtr<Text::StringBuilderUTF8> sb);

			virtual void BeginUpdate();
			virtual void EndUpdate();
			virtual UOSInt AddItem(NotNullPtr<Text::String> itemText, void *itemObj);
			virtual UOSInt AddItem(Text::CStringNN itemText, void *itemObj);
			virtual UOSInt InsertItem(UOSInt index, NotNullPtr<Text::String> itemText, void *itemObj);
			virtual UOSInt InsertItem(UOSInt index, Text::CStringNN itemText, void *itemObj);
			virtual void *RemoveItem(UOSInt index);
			virtual void ClearItems();
			virtual UOSInt GetCount();
			virtual void SetSelectedIndex(UOSInt index);
			virtual UOSInt GetSelectedIndex();
			virtual void *GetSelectedItem();
			virtual void *GetItem(UOSInt index);

			virtual Math::Size2DDbl GetSize();
			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
			virtual void SetTextSelection(UOSInt startPos, UOSInt endPos);
		};
	}
}
#endif
