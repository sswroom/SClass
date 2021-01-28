#ifndef _SM_UI_GTK_GTKLABEL
#define _SM_UI_GTK_GTKLABEL
#include "Data/ArrayList.h"
#include "UI/Gtk/GtkControl.h"
#include "UI/Gtk/GtkUI.h"

namespace UI
{
	namespace Gtk
	{
		class GtkLabel : public GtkControl
		{
		private:
			Bool hasTextColor;
			Int32 textColor;
		public:
			GtkLabel(GtkUI *ui, GtkClientControl *parent, const WChar *label);
			virtual ~GtkLabel();

			virtual void SetText(const WChar *text);

			virtual const WChar *GetObjectClass();
			virtual OSInt OnNotify(Int32 code, void *lParam);
			Int32 GetTextColor();
			void SetTextColor(Int32 textColor);
		};
	}
}
#endif
