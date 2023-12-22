#ifndef _SM_UI_WIN_WINLISTBOX
#define _SM_UI_WIN_WINLISTBOX
#include "UI/GUIListBox.h"
#include <windows.h>

namespace UI
{
	namespace Win
	{
		class WinListBox : public GUIListBox
		{
		public:
			struct ItemData;

		private:
			WNDPROC wndproc;
			Data::ArrayList<ItemData *> items;
			Bool mulSel;

			static OSInt __stdcall LBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		public:
			WinListBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Bool multiSelect);
			virtual ~WinListBox();

			virtual UOSInt AddItem(NotNullPtr<Text::String> itemText, void *itemObj);
			virtual UOSInt AddItem(Text::CStringNN itemText, void *itemObj);
			UOSInt AddItem(const WChar *itemText, void *itemObj);
			virtual UOSInt InsertItem(UOSInt index, Text::String *itemText, void *itemObj);
			virtual UOSInt InsertItem(UOSInt index, Text::CStringNN itemText, void *itemObj);
			UOSInt InsertItem(UOSInt index, const WChar *itemText, void *itemObj);
			virtual void *RemoveItem(UOSInt index);
			virtual void *GetItem(UOSInt index);
			virtual void ClearItems();
			virtual UOSInt GetCount();
			virtual void SetSelectedIndex(UOSInt index);
			virtual UOSInt GetSelectedIndex();
			virtual Bool GetSelectedIndices(Data::ArrayList<UInt32> *indices);
			virtual void *GetSelectedItem();
			virtual UTF8Char *GetSelectedItemText(UTF8Char *buff);
			WChar *GetSelectedItemText(WChar *buff);
			virtual Optional<Text::String> GetSelectedItemTextNew();
			virtual UTF8Char *GetItemText(UTF8Char *buff, UOSInt index);
			WChar *GetItemText(WChar *buff, UOSInt index);
			virtual void SetItemText(UOSInt index, Text::CStringNN text);
			virtual Optional<Text::String> GetItemTextNew(UOSInt index);
			virtual OSInt GetItemHeight();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
