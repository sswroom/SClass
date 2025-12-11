#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/JavaUI/JavaDateTimePicker.h"

UI::JavaUI::JavaDateTimePicker::JavaDateTimePicker(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIDateTimePicker(ui, parent)
{
}

UI::JavaUI::JavaDateTimePicker::~JavaDateTimePicker()
{
}

OSInt UI::JavaUI::JavaDateTimePicker::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JavaDateTimePicker::SetValue(NN<Data::DateTime> dt)
{
}

void UI::JavaUI::JavaDateTimePicker::SetValue(const Data::Timestamp &ts)
{
}

void UI::JavaUI::JavaDateTimePicker::GetSelectedTime(NN<Data::DateTime> dt)
{
}

Data::Timestamp UI::JavaUI::JavaDateTimePicker::GetSelectedTime()
{
	return nullptr;
}

void UI::JavaUI::JavaDateTimePicker::SetFormat(const Char *format)
{
}

void UI::JavaUI::JavaDateTimePicker::SetCalShowWeeknum(Bool showWeeknum)
{
}
