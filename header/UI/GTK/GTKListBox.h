#ifndef _SM_UI_GTK_GTKLISTBOX
#define _SM_UI_GTK_GTKLISTBOX
#include "UI/GUIListBox.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKListBox : public UI::GUIListBox
		{
		public:
			struct ItemData;
		private:
			Data::ArrayListNN<ItemData> items;
			Bool mulSel;
			GtkWidget *listbox;
			Bool isShown;
			Int64 showTime;

			static void SignalSelChange(GtkListBox *listBox, GtkListBoxRow *row, gpointer data);
			static gboolean SignalButtonPress(GtkWidget *widget, GdkEvent *event, gpointer data);
			static void SignalShow(GtkWidget *widget, gpointer user_data);
		public:
			GTKListBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool multiSelect);
			virtual ~GTKListBox();

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
			virtual Bool GetSelectedIndices(NN<Data::ArrayListNative<UInt32>> indices);
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
