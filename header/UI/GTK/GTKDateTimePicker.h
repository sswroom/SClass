#ifndef _SM_UI_GTK_GTKDATETIMEPICKER
#define _SM_UI_GTK_GTKDATETIMEPICKER
#include "UI/GUIDateTimePicker.h"
#include <gtk/gtk.h>

namespace UI
{
	namespace GTK
	{
		class GTKDateTimePicker : public GUIDateTimePicker
		{
		private:
			static Int32 useCnt;
			Bool showWeeknum;
			GtkWidget *widget;
			UnsafeArrayOpt<const Char> format;

		public:
			GTKDateTimePicker(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~GTKDateTimePicker();

			virtual IntOS OnNotify(UInt32 code, void *lParam);
			virtual void SetValue(NN<Data::DateTime> dt);
			virtual void SetValue(const Data::Timestamp &dt);
			virtual void GetSelectedTime(NN<Data::DateTime> dt);
			virtual Data::Timestamp GetSelectedTime();
			virtual void SetFormat(const Char *format);
			virtual void SetCalShowWeeknum(Bool showWeeknum);
		};
	}
}

#endif
