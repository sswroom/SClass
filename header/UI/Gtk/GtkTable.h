#ifndef _SM_UI_GTK_GTKTABLE
#define _SM_UI_GTK_GTKTABLE
#include "Data/ArrayList.h"
#include "UI/Gtk/GtkControl.h"
#include "UI/Gtk/GtkUI.h"

namespace UI
{
	namespace Gtk
	{
		class GtkTable : public GtkControl
		{
		private:
			void *tableWnd;
			Bool hasTextColor;
			OSInt rowCount;
		public:
			GtkTable(GtkUI *ui, GtkClientControl *parent, Int32 colCount);
			virtual ~GtkTable();

			virtual void SetText(const WChar *text);

			virtual const WChar *GetObjectClass();
			virtual OSInt OnNotify(UInt32 code, void *lParam);
			OSInt AddItem(const WChar *text);
			OSInt SetSubItem(OSInt row, OSInt col, const WChar *text);
		};
	}
}
#endif
