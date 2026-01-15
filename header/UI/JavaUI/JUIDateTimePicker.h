#ifndef _SM_UI_JAVAUI_JUIDATETIMEPICKER
#define _SM_UI_JAVAUI_JUIDATETIMEPICKER
#include "UI/GUIDateTimePicker.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIDateTimePicker : public GUIDateTimePicker
		{
		public:
			JUIDateTimePicker(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
			virtual ~JUIDateTimePicker();

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
