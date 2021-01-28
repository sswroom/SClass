#ifndef _SM_UI_GTK_GTKUI
#define _SM_UI_GTK_GTKUI
#include "UI/IWindowSystem.h"

namespace UI
{
	namespace Gtk
	{
		class GtkUI
		{
		public:
			GtkUI();
			virtual ~GtkUI();

			virtual void Run();
			virtual void ProcessMessages();
			virtual void Exit();
		};
	}
}
#endif
