#ifndef _SM_UI_GTK_GTKTEXTVIEW
#define _SM_UI_GTK_GTKTEXTVIEW
#include "Data/ArrayList.h"
#include "UI/Gtk/GtkControl.h"
#include "UI/Gtk/GtkUI.h"

namespace UI
{
	namespace Gtk
	{
		class GtkTextView : public GtkControl
		{
		private:
			Bool hasTextColor;
			Int32 textColor;
			void *txtView;
			void *buff;
		public:
			GtkTextView(GtkUI *ui, GtkClientControl *parent, const UTF8Char *label);
			virtual ~GtkTextView();

			virtual void SetText(const UTF8Char *text);

			virtual const UTF8Char *GetObjectClass();
			virtual OSInt OnNotify(Int32 code, void *lParam);
			void SetReadOnly(Bool readOnly);
			Int32 GetTextColor();
			void SetTextColor(Int32 textColor);
		};
	}
}
#endif
