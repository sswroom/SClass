#ifndef _SM_UI_GTK_GTKTIMER
#define _SM_UI_GTK_GTKTIMER
#include "UI/Gtk/GtkUI.h"

namespace UI
{
	namespace Gtk
	{
		class GtkTimer
		{
		private:
			Int32 interval;
			UI::UIEvent handler;
			void *userObj;
			UInt32 id;
		public:
			GtkTimer(Int32 interval, UI::UIEvent handler, void *userObj);
			~GtkTimer();
		private:
			static Int32 OnTick(void *userObj);
		};
	}
}
#endif
