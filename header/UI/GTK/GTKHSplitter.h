#ifndef _SM_UI_GTK_GTKHSPLITTER
#define _SM_UI_GTK_GTKHSPLITTER
#include "UI/GUIHSplitter.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKHSplitter : public UI::GUIHSplitter
		{
		private:
			Bool isRight;
			Bool dragMode;
			IntOS dragX;
			IntOS dragY;

			static gboolean SignalMouseDown(GtkWidget *widget, GdkEvent *event, gpointer data);
			static gboolean SignalMouseUp(GtkWidget *widget, GdkEvent *event, gpointer data);
		public:
			GTKHSplitter(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Int32 width, Bool isRight);
			virtual ~GTKHSplitter();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<IntOS> pos);
		};
	}
}
#endif
