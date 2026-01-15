#ifndef _SM_UI_GTK_GTKBUTTON
#define _SM_UI_GTK_GTKBUTTON
#include "UI/GUIButton.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKButton : public UI::GUIButton
		{
		private:
			static void SignalClicked(void *window, void *userObj);
			static void SignalPress(void *window, void *userObj);
			static void SignalRelease(void *window, void *userObj);
			static gboolean SignalFocus(void *window, GtkDirectionType direction, void *userObj);
			static gboolean SignalFocusLost(void *window, void *userObj);
		public:
			GTKButton(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN txt);
			virtual ~GTKButton();

			virtual void SetText(Text::CStringNN text);
			virtual void SetFont(Text::CString fontName, Double fontHeightPt, Bool isBold);

			virtual IntOS OnNotify(UInt32 code, void *lParam);
		};
	}
}
#endif
