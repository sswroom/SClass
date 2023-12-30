#ifndef _SM_UI_JAVA_JAVADATETIMEPICKER
#define _SM_UI_JAVA_JAVADATETIMEPICKER
#include "UI/GUIDateTimePicker.h"

namespace UI
{
	namespace Java
	{
		class JavaDateTimePicker : public GUIDateTimePicker
		{
		public:
			JavaDateTimePicker(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent);
			virtual ~JavaDateTimePicker();

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
