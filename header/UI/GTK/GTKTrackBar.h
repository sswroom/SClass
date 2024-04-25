#ifndef _SM_UI_GTK_GTKTRACKBAR
#define _SM_UI_GTK_GTKTRACKBAR
#include "UI/GUITrackBar.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKTrackBar : public GUITrackBar
		{
		private:
			static gboolean SignalValueChanged(void *window, void *userObj);
		public:
			GTKTrackBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UOSInt minVal, UOSInt maxVal, UOSInt currVal);
			virtual ~GTKTrackBar();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void SetPos(UOSInt pos);
			virtual void SetRange(UOSInt minVal, UOSInt maxVal);
			virtual UOSInt GetPos();
		};
	}
}
#endif
