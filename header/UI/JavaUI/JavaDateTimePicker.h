#ifndef _SM_UI_JAVAUI_JAVADATETIMEPICKER
#define _SM_UI_JAVAUI_JAVADATETIMEPICKER
#include "UI/GUIDateTimePicker.h"

namespace UI
{
	namespace JavaUI
	{
		class JavaDateTimePicker : public GUIDateTimePicker
		{
		public:
			JavaDateTimePicker(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~JavaDateTimePicker();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
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
