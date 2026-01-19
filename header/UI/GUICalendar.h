#ifndef _SM_UI_GUICALENDAR
#define _SM_UI_GUICALENDAR
#include "Data/ArrayListObj.hpp"
#include "Data/DateTime.h"
#include "UI/GUIClientControl.h"

namespace UI
{
	class GUICalendar : public GUIControl
	{
	public:
		typedef void (CALLBACKFUNC DateChangedHandler)(AnyType userObj, NN<Data::DateTime> newDate);
	private:
		static Int32 useCnt;

		Data::ArrayListObj<DateChangedHandler> dateChangedHdlrs;
		Data::ArrayListObj<AnyType> dateChangedObjs;
	public:
		GUICalendar(NN<GUICore> ui, Optional<UI::GUIClientControl> parent);
		virtual ~GUICalendar();

		virtual Text::CStringNN GetObjectClass();
		virtual IntOS OnNotify(UInt32 code, void *lParam);

		void GetSelectedTime(NN<Data::DateTime> dt);
		void HandleDateChange(DateChangedHandler hdlr, AnyType obj);
	};
}
#endif
