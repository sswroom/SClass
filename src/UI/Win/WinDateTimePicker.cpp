#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Win/WinCore.h"
#include "UI/Win/WinDateTimePicker.h"
#include <windows.h>
#include <commctrl.h>

Int32 UI::Win::WinDateTimePicker::useCnt = 0;

UI::Win::WinDateTimePicker::WinDateTimePicker(NN<GUICore> ui, NN<UI::GUIClientControl> parent, Bool calendarSel) : UI::GUIDateTimePicker(ui, parent)
{
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(icex);
		icex.dwICC = ICC_DATE_CLASSES;
		InitCommonControlsEx(&icex);
	}
	this->showWeeknum = false;

	UInt32 style = WS_BORDER | WS_TABSTOP | WS_CHILD;
	if (!calendarSel)
	{
		style = style | DTS_UPDOWN;
	}
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((Win::WinCore*)ui.Ptr())->GetHInst(), parent, DATETIMEPICK_CLASS, (const UTF8Char*)"", style, WS_EX_CLIENTEDGE, 0, 0, 200, 200);
	this->SetFormat("yyyy-MM-dd HH:mm:ss");
}

UI::Win::WinDateTimePicker::~WinDateTimePicker()
{
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
	}
}

OSInt UI::Win::WinDateTimePicker::OnNotify(UInt32 code, void *lParam)
{
	Data::DateTime dt;
	LPNMDATETIMECHANGE chg;

	switch (code)
	{
	case DTN_DATETIMECHANGE:
		chg = (LPNMDATETIMECHANGE)lParam;
		dt.SetValueSYSTEMTIME(&chg->st);
		this->EventDateChange(dt);
		break;
	case DTN_DROPDOWN:
		{
//			NMHDR *nmhdr = (NMHDR*)lParam;
			if (this->showWeeknum)
			{
				HWND hwndCal = (HWND)SendMessage((HWND)this->hwnd.OrNull(), DTM_GETMONTHCAL, 0, 0);
				RECT rcCal;
				if (hwndCal == 0)
					return 0;
				UInt32 style = (UInt32)GetWindowLong(hwndCal, GWL_STYLE);
				style = style | MCS_WEEKNUMBERS;
				SetWindowLong(hwndCal, GWL_STYLE, (Int32)style);
				GetWindowRect(hwndCal, &rcCal);
				MoveWindow(hwndCal, rcCal.left, rcCal.top, rcCal.right - rcCal.left + 30, rcCal.bottom - rcCal.top, FALSE);
			}
		}
		break;
	}
	return 0;
}

void UI::Win::WinDateTimePicker::SetValue(NN<Data::DateTime> dt)
{
	SYSTEMTIME t;
	Int8 tz = dt->GetTimeZoneQHR();
	dt->SetTimeZoneQHR(0);
	dt->ToSYSTEMTIME(&t);
	dt->SetTimeZoneQHR(tz);
	SendMessage((HWND)this->hwnd.OrNull(), DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&t);
}

void UI::Win::WinDateTimePicker::SetValue(const Data::Timestamp &ts)
{
	SYSTEMTIME t;
	ts.ToSYSTEMTIME(&t);
	SendMessage((HWND)this->hwnd.OrNull(), DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&t);
}

void UI::Win::WinDateTimePicker::GetSelectedTime(NN<Data::DateTime> dt)
{
	SYSTEMTIME t;
	SendMessage((HWND)this->hwnd.OrNull(), DTM_GETSYSTEMTIME, 0, (LPARAM)&t);
	Int8 tz = dt->GetTimeZoneQHR();
	dt->ToUTCTime();
	dt->SetValueSYSTEMTIME(&t);
	dt->SetTimeZoneQHR(tz);
}

Data::Timestamp UI::Win::WinDateTimePicker::GetSelectedTime()
{
	SYSTEMTIME t;
	SendMessage((HWND)this->hwnd.OrNull(), DTM_GETSYSTEMTIME, 0, (LPARAM)&t);
	return Data::Timestamp(Data::DateTimeUtil::SYSTEMTIME2Ticks(&t), 0);
}

void UI::Win::WinDateTimePicker::SetFormat(const Char *format)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew((const UTF8Char*)format);
	SendMessage((HWND)this->hwnd.OrNull(), DTM_SETFORMATW, 0, (LPARAM)wptr.Ptr());
	Text::StrDelNew(wptr);
}

void UI::Win::WinDateTimePicker::SetCalShowWeeknum(Bool showWeeknum)
{
	this->showWeeknum = showWeeknum;
}

