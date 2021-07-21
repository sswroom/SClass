#ifndef _SM_UI_GTK_GTKCOMBOBOX
#define _SM_UI_GTK_GTKCOMBOBOX
#include "Data/ArrayList.h"
#include "UI/Gtk/GtkControl.h"
#include "UI/Gtk/GtkUI.h"

namespace UI
{
	namespace Gtk
	{
		class GtkComboBox : public GtkControl
		{
		private:
			Data::ArrayList<UI::UIEvent> *selChgHdlrs;
			Data::ArrayList<void *> *selChgObjs;
			void *store;
			Data::ArrayList<void *> *items;
			Data::ArrayList<const WChar *> *texts;

			static void EventChanged(void *window, void *userObj);
		public:
			GtkComboBox(GtkUI *ui, GtkClientControl *parent, Bool allowEditing);
			virtual ~GtkComboBox();

			virtual void SetText(const WChar *text);

			OSInt AddItem(const WChar *itemText, void *itemObj);
			OSInt InsertItem(OSInt index, const WChar *itemText, void *itemObj);
			void *RemoveItem(OSInt index);
			void ClearItems();
			OSInt GetCount();
			void SetSelectedIndex(OSInt index);
			OSInt GetSelectedIndex();
			WChar *GetSelectedItemText(WChar *sbuff);
			void *GetSelectedItem();
			WChar *GetItemText(WChar *buff, OSInt index);
			void *GetItem(OSInt index);

			virtual const WChar *GetObjectClass();
			virtual OSInt OnNotify(UInt32 code, void *lParam);

			virtual void HandleSelectionChange(UI::UIEvent hdlr, void *userObj);
		};
	}
}
#endif
