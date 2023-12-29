#ifndef _SM_UI_GTK_GTKRADIOBUTTON
#define _SM_UI_GTK_GTKRADIOBUTTON
#include "UI/GUIRadioButton.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKRadioButton : public GUIRadioButton
		{
		private:
			Bool selected;

			static void SignalToggled(GtkRadioButton *btn, gpointer data);
			void ChangeSelected(Bool selVal);
		public:
			GTKRadioButton(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Text::CStringNN initText, Bool selected);
			virtual ~GTKRadioButton();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual Bool IsSelected();
			virtual void Select();
		};
	}
}
#endif
