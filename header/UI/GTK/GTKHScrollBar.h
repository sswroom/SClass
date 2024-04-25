#ifndef _SM_UI_GTK_GTKHSCROLLBAR
#define _SM_UI_GTK_GTKHSCROLLBAR
#include "UI/GUIHScrollBar.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKHScrollBar : public GUIHScrollBar
		{
		private:
			static gboolean SignalValueChanged(void *window, void *userObj);
		public:
			GTKHScrollBar(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Double width);
			virtual ~GTKHScrollBar();

			virtual void InitScrollBar(UOSInt minVal, UOSInt maxVal, UOSInt currVal, UOSInt largeChg);
			virtual void SetPos(UOSInt pos);
			virtual UOSInt GetPos();
			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual void SetAreaP(OSInt left, OSInt top, OSInt right, OSInt bottom, Bool updateScn);
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
		};
	}
}
#endif
