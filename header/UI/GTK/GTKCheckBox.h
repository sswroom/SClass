#ifndef _SM_UI_GTK_GTKCHECKBOX
#define _SM_UI_GTK_GTKCHECKBOX
#include "UI/GUICheckBox.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKCheckBox : public GUICheckBox
		{
		private:
			static void SignalToggled(GtkToggleButton *btn, gpointer data);
		public:
			GTKCheckBox(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN initText, Bool checked);
			virtual ~GTKCheckBox();

			virtual void SetText(Text::CStringNN text);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual Bool IsChecked();
			virtual void SetChecked(Bool checked);
		};
	}
}
#endif
