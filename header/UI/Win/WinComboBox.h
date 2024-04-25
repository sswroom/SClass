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
			UOSInt minVisible;
			Bool allowEdit;

		public:
			WinComboBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool allowEditing);
			virtual ~WinComboBox();

			virtual void SetText(Text::CStringNN text);
			virtual UTF8Char *GetText(UTF8Char *buff);
			virtual Bool GetText(NN<Text::StringBuilderUTF8> sb);

			virtual void BeginUpdate();
			virtual void EndUpdate();
			virtual UOSInt AddItem(NN<Text::String> itemText, AnyType itemObj);
			virtual UOSInt AddItem(Text::CStringNN itemText, AnyType itemObj);
			virtual UOSInt InsertItem(UOSInt index, NN<Text::String> itemText, AnyType itemObj);
			virtual UOSInt InsertItem(UOSInt index, Text::CStringNN itemText, AnyType itemObj);
			virtual AnyType RemoveItem(UOSInt index);
			virtual void ClearItems();
			virtual UOSInt GetCount();
			virtual void SetSelectedIndex(UOSInt index);
			virtual UOSInt GetSelectedIndex();
			virtual AnyType GetSelectedItem();
			virtual AnyType GetItem(UOSInt index);

			virtual Math::Size2DDbl GetSize();
			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
			virtual void SetTextSelection(UOSInt startPos, UOSInt endPos);

			UOSInt GetListMinVisible();
			Bool SetListMinVisible(UOSInt itemCount);
			UOSInt GetSelectionHeight();
			Bool SetListItemHeight(UOSInt itemHeight);
		};

	}
}
#endif
