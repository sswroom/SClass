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
			GTKTrackBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, UIntOS minVal, UIntOS maxVal, UIntOS currVal);
			virtual ~GTKTrackBar();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void SetPos(UIntOS pos);
			virtual void SetRange(UIntOS minVal, UIntOS maxVal);
			virtual UIntOS GetPos();
		};
	}
}
#endif
