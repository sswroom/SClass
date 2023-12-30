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
			WinDateTimePicker(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, Bool calendarSelect);
			virtual ~WinDateTimePicker();

			virtual OSInt OnNotify(UInt32 code, void *lParam);
			virtual void SetValue(NotNullPtr<Data::DateTime> dt);
			virtual void SetValue(const Data::Timestamp &dt);
			virtual void GetSelectedTime(NotNullPtr<Data::DateTime> dt);
			virtual Data::Timestamp GetSelectedTime();
			virtual void SetFormat(const Char *format);
			virtual void SetCalShowWeeknum(Bool showWeeknum);
		};
	}
}

#endif
