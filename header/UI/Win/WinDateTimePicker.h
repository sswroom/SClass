#ifndef _SM_UI_WIN_WINDATETIMEPICKER
#define _SM_UI_WIN_WINDATETIMEPICKER
#include "UI/GUIDateTimePicker.h"

namespace UI
{
	namespace Win
	{
		class WinDateTimePicker : public GUIDateTimePicker
		{
		private:
			static Int32 useCnt;
			Bool showWeeknum;

		public:
			WinDateTimePicker(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool calendarSelect);
			virtual ~WinDateTimePicker();

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
