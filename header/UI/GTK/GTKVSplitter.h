#ifndef _SM_UI_GTK_GTKVSPLITTER
#define _SM_UI_GTK_GTKVSPLITTER
#include "UI/GUIVSplitter.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKVSplitter : public GUIVSplitter
		{
		private:
			Bool isBottom;
			Bool dragMode;
			OSInt dragX;
			OSInt dragY;

			static gboolean SignalMouseDown(GtkWidget *widget, GdkEvent *event, gpointer data);
			static gboolean SignalMouseUp(GtkWidget *widget, GdkEvent *event, gpointer data);
		public:
			GTKVSplitter(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Int32 height, Bool isBottom);
			virtual ~GTKVSplitter();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void EventMouseDown(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
			virtual void EventMouseUp(UI::GUIControl::MouseButton btn, Math::Coord2D<OSInt> pos);
		};
	}
}
#endif
