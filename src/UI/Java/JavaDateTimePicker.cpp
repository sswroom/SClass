#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/Java/JavaDateTimePicker.h"

UI::Java::JavaDateTimePicker::JavaDateTimePicker(NotNullPtr<UI::GUICore> ui, NotNullPtr<UI::GUIClientControl> parent) : UI::GUIDateTimePicker(ui, parent)
{
}

UI::Java::JavaDateTimePicker::~JavaDateTimePicker()
{
}

OSInt UI::Java::JavaDateTimePicker::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::Java::JavaDateTimePicker::SetValue(NotNullPtr<Data::DateTime> dt)
{
}

void UI::Java::JavaDateTimePicker::SetValue(const Data::Timestamp &ts)
{
}

void UI::Java::JavaDateTimePicker::GetSelectedTime(NotNullPtr<Data::DateTime> dt)
{
}

Data::Timestamp UI::Java::JavaDateTimePicker::GetSelectedTime()
{
}

void UI::Java::JavaDateTimePicker::SetFormat(const Char *format)
{
}

void UI::Java::JavaDateTimePicker::SetCalShowWeeknum(Bool showWeeknum)
{
}