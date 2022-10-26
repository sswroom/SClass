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
		typedef void (__stdcall *DateChangedHandler)(void *userObj, Data::DateTime *newDate);
		typedef enum
		{
			ST_UPDOWN,
			ST_CALENDAR
		} SelectType;
	private:
		static Int32 useCnt;
		Bool showWeeknum;
		void *clsData;

		Data::ArrayList<DateChangedHandler> *dateChangedHdlrs;
		Data::ArrayList<void*> *dateChangedObjs;

	public:
		GUIDateTimePicker(GUICore *ui, UI::GUIClientControl *parent, SelectType st);
		virtual ~GUIDateTimePicker();

		virtual Text::CString GetObjectClass();
		virtual OSInt OnNotify(UInt32 code, void *lParam);

		void SetValue(Data::DateTime *dt);
		void SetValue(Data::Timestamp dt);
		void GetSelectedTime(Data::DateTime *dt);
		Data::Timestamp GetSelectedTime();
		void SetFormat(const Char *format);
		void SetCalShowWeeknum(Bool showWeeknum);
		void HandleDateChange(DateChangedHandler hdlr, void *obj);
	};
}

#endif
