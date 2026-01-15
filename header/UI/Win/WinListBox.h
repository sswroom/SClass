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
			Data::ArrayListNN<ItemData> items;
			Bool mulSel;

			static IntOS __stdcall LBWndProc(void *hWnd, UInt32 msg, UIntOS wParam, IntOS lParam);
		public:
			WinListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool multiSelect);
			virtual ~WinListBox();

			virtual UIntOS AddItem(NN<Text::String> itemText, AnyType itemObj);
			virtual UIntOS AddItem(Text::CStringNN itemText, AnyType itemObj);
			UIntOS AddItem(const WChar *itemText, AnyType itemObj);
			virtual UIntOS InsertItem(UIntOS index, NN<Text::String> itemText, AnyType itemObj);
			virtual UIntOS InsertItem(UIntOS index, Text::CStringNN itemText, AnyType itemObj);
			UIntOS InsertItem(UIntOS index, const WChar *itemText, AnyType itemObj);
			virtual AnyType RemoveItem(UIntOS index);
			virtual AnyType GetItem(UIntOS index);
			virtual void ClearItems();
			virtual UIntOS GetCount();
			virtual void SetSelectedIndex(UIntOS index);
			virtual UIntOS GetSelectedIndex();
			virtual Bool GetSelectedIndices(NN<Data::ArrayList<UInt32>> indices);
			virtual AnyType GetSelectedItem();
			virtual UnsafeArrayOpt<UTF8Char> GetSelectedItemText(UnsafeArray<UTF8Char> buff);
			WChar *GetSelectedItemText(WChar *buff);
			virtual Optional<Text::String> GetSelectedItemTextNew();
			virtual UnsafeArrayOpt<UTF8Char> GetItemText(UnsafeArray<UTF8Char> buff, UIntOS index);
			WChar *GetItemText(WChar *buff, UIntOS index);
			virtual void SetItemText(UIntOS index, Text::CStringNN text);
			virtual Optional<Text::String> GetItemTextNew(UIntOS index);
			virtual IntOS GetItemHeight();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
