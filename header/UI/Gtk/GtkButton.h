#ifndef _SM_UI_GTK_GTKBUTTON
#define _SM_UI_GTK_GTKBUTTON
#include "Data/ArrayList.h"
#include "UI/Gtk/GtkControl.h"
#include "UI/Gtk/GtkUI.h"

namespace UI
{
	namespace Gtk
	{
		class GtkButton : public GtkControl
		{
		private:
			Data::ArrayList<UIEvent> *btnClkHandlers;
			Data::ArrayList<void *> *btnClkHandlerObjs;

			static void EventClicked(void *window, void *userObj);
		public:
			GtkButton(GtkUI *ui, GtkClientControl *parent, const WChar *label);
			virtual ~GtkButton();

			virtual void SetText(const WChar *text);
			virtual void SetFont(const WChar *name, Single size, Bool isBold);

			virtual const WChar *GetObjectClass();
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			
			void HandleButtonClick(UIEvent handler, void *userObj);
		};
	}
}
#endif
