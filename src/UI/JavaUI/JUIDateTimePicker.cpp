#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/JavaUI/JUIDateTimePicker.h"

UI::JavaUI::JUIDateTimePicker::JUIDateTimePicker(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent) : UI::GUIDateTimePicker(ui, parent)
{
}

UI::JavaUI::JUIDateTimePicker::~JUIDateTimePicker()
{
}

OSInt UI::JavaUI::JUIDateTimePicker::OnNotify(UInt32 code, void *lParam)
{
	return 0;
}

void UI::JavaUI::JUIDateTimePicker::SetValue(NN<Data::DateTime> dt)
{
}

void UI::JavaUI::JUIDateTimePicker::SetValue(const Data::Timestamp &ts)
{
}

void UI::JavaUI::JUIDateTimePicker::GetSelectedTime(NN<Data::DateTime> dt)
{
}

Data::Timestamp UI::JavaUI::JUIDateTimePicker::GetSelectedTime()
{
	return nullptr;
}

void UI::JavaUI::JUIDateTimePicker::SetFormat(const Char *format)
{
}

void UI::JavaUI::JUIDateTimePicker::SetCalShowWeeknum(Bool showWeeknum)
{
}
