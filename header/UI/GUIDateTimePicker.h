#ifndef _SM_UI_GUIDATETIMEPICKER
#define _SM_UI_GUIDATETIMEPICKER
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "Data/Timestamp.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIDateTimePicker : public GUIControl
	{
	public:
		typedef void (__stdcall *DateChangedHandler)(void *userObj, NotNullPtr<Data::DateTime> newDate);
	private:
		Data::ArrayList<DateChangedHandler> dateChangedHdlrs;
		Data::ArrayList<void*> dateChangedObjs;

	public:
		GUIDateTimePicker(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
		virtual ~GUIDateTimePicker();

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual void SetValue(NotNullPtr<Data::DateTime> dt) = 0;
		virtual void SetValue(const Data::Timestamp &dt) = 0;
		virtual void GetSelectedTime(NotNullPtr<Data::DateTime> dt) = 0;
		virtual Data::Timestamp GetSelectedTime() = 0;
		virtual void SetFormat(const Char *format) = 0;
		virtual void SetCalShowWeeknum(Bool showWeeknum) = 0;

		virtual Text::CStringNN GetObjectClass() const;
		void EventDateChange(NotNullPtr<Data::DateTime> newDate);
		void HandleDateChange(DateChangedHandler hdlr, void *obj);
	};
}

#endif
