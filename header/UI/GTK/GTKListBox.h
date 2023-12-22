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
			Data::ArrayList<ItemData *> items;
			Bool mulSel;
			GtkWidget *listbox;
			Bool isShown;
			Int64 showTime;

			static void SignalSelChange(GtkListBox *listBox, GtkListBoxRow *row, gpointer data);
			static gboolean SignalButtonPress(GtkWidget *widget, GdkEvent *event, gpointer data);
			static void SignalShow(GtkWidget *widget, gpointer user_data);
			static OSInt __stdcall LBWndProc(void *hWnd, UInt32 msg, UOSInt wParam, OSInt lParam);
		public:
			GTKListBox(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Bool multiSelect);
			virtual ~GTKListBox();

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
