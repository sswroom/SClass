#ifndef _SM_UI_GTK_GTKTABCONTROL
#define _SM_UI_GTK_GTKTABCONTROL
#include "UI/GUITabControl.h"
#include <gtk/gtk.h>

namespace UI
{
	class GUITabPage;

	namespace GTK
	{
		class GTKTabControl : public GUITabControl
		{
		private:
			Data::ArrayListNN<UI::GUITabPage> tabPages;
			UOSInt selIndex;

			static void SignalSwitchPage(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data);
			static gboolean SignalShown(gpointer user_data);
		public:
			GTKTabControl(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
			virtual ~GTKTabControl();

			virtual NotNullPtr<GUITabPage> AddTabPage(NotNullPtr<Text::String> itemText);
			virtual NotNullPtr<GUITabPage> AddTabPage(Text::CStringNN itemText);
			virtual void SetSelectedIndex(UOSInt index);
			virtual void SetSelectedPage(NotNullPtr<GUITabPage> page);
			virtual UOSInt GetSelectedIndex();
			virtual Optional<GUITabPage> GetSelectedPage();
			virtual void SetTabPageName(UOSInt index, Text::CStringNN name);
			virtual UTF8Char *GetTabPageName(UOSInt index, UTF8Char *buff);
			virtual Math::RectArea<OSInt> GetTabPageRect();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void OnSizeChanged(Bool updateScn);
			virtual void SetDPI(Double hdpi, Double ddpi);
		};
	}
}
#endif
