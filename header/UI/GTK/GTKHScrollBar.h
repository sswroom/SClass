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

			virtual void InitScrollBar(UIntOS minVal, UIntOS maxVal, UIntOS currVal, UIntOS largeChg);
			virtual void SetPos(UIntOS pos);
			virtual UIntOS GetPos();
			virtual void SetArea(Double left, Double top, Double right, Double bottom, Bool updateScn);
			virtual void SetAreaP(IntOS left, IntOS top, IntOS right, IntOS bottom, Bool updateScn);
			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void UpdatePos(Bool redraw);
		};
	}
}
#endif
