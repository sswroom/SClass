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
			UIntOS selIndex;

			static void SignalSwitchPage(GtkNotebook *notebook, GtkWidget *page, guint page_num, gpointer user_data);
			static gboolean SignalShown(gpointer user_data);
		public:
			GTKTabControl(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~GTKTabControl();

			virtual NN<GUITabPage> AddTabPage(NN<Text::String> itemText);
			virtual NN<GUITabPage> AddTabPage(Text::CStringNN itemText);
			virtual void SetSelectedIndex(UIntOS index);
			virtual void SetSelectedPage(NN<GUITabPage> page);
			virtual UIntOS GetSelectedIndex();
			virtual Optional<GUITabPage> GetSelectedPage();
			virtual void SetTabPageName(UIntOS index, Text::CStringNN name);
			virtual UnsafeArrayOpt<UTF8Char> GetTabPageName(UIntOS index, UnsafeArray<UTF8Char> buff);
			virtual Math::RectArea<IntOS> GetTabPageRect();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void OnSizeChanged(Bool updateScn);
			virtual void SetDPI(Double hdpi, Double ddpi);
		};
	}
}
#endif
