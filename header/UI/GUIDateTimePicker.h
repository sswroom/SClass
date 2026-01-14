#ifndef _SM_UI_GUIDATETIMEPICKER
#define _SM_UI_GUIDATETIMEPICKER
#include "AnyType.h"
#include "Data/ArrayListObj.hpp"
#include "Data/CallbackStorage.h"
#include "Data/DateTime.h"
#include "Data/Timestamp.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUIDateTimePicker : public GUIControl
	{
	public:
		typedef void (CALLBACKFUNC DateChangedHandler)(AnyType userObj, NN<Data::DateTime> newDate);
	private:
		Data::ArrayListObj<Data::CallbackStorage<DateChangedHandler>> dateChangedHdlrs;

	public:
		GUIDateTimePicker(NN<GUICore> ui, NN<UI::GUIClientControl> parent);
		virtual ~GUIDateTimePicker();

		virtual OSInt OnNotify(UInt32 code, void *lParam) = 0;
		virtual void SetValue(NN<Data::DateTime> dt) = 0;
		virtual void SetValue(const Data::Timestamp &dt) = 0;
		virtual void GetSelectedTime(NN<Data::DateTime> dt) = 0;
		virtual Data::Timestamp GetSelectedTime() = 0;
		virtual void SetFormat(const Char *format) = 0;
		virtual void SetCalShowWeeknum(Bool showWeeknum) = 0;

		virtual Text::CStringNN GetObjectClass() const;
		void EventDateChange(NN<Data::DateTime> newDate);
		void HandleDateChange(DateChangedHandler hdlr, AnyType obj);
	};
}

#endif
