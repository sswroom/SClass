#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUICoreWin.h"
#include "UI/GUIDateTimePicker.h"
#include <windows.h>
#include <commctrl.h>

Int32 UI::GUIDateTimePicker::useCnt = 0;

UI::GUIDateTimePicker::GUIDateTimePicker(NotNullPtr<GUICore> ui, NotNullPtr<UI::GUIClientControl> parent, SelectType st) : UI::GUIControl(ui, parent)
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
	if (st == ST_UPDOWN)
	{
		style = style | DTS_UPDOWN;
	}
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((GUICoreWin*)ui.Ptr())->GetHInst(), parent, DATETIMEPICK_CLASS, (const UTF8Char*)"", style, WS_EX_CLIENTEDGE, 0, 0, 200, 200);
	this->SetFormat("yyyy-MM-dd HH:mm:ss");
}

UI::GUIDateTimePicker::~GUIDateTimePicker()
{
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
	}
}

Text::CStringNN UI::GUIDateTimePicker::GetObjectClass() const
{
	return CSTR("DateTimePicker");
}

OSInt UI::GUIDateTimePicker::OnNotify(UInt32 code, void *lParam)
{
	Data::DateTime dt;
	UOSInt i;
	LPNMDATETIMECHANGE chg;

	switch (code)
	{
	case DTN_DATETIMECHANGE:
		chg = (LPNMDATETIMECHANGE)lParam;
		dt.SetValueSYSTEMTIME(&chg->st);
		i = this->dateChangedHdlrs.GetCount();
		while (i-- > 0)
		{
			this->dateChangedHdlrs.GetItem(i)(this->dateChangedObjs.GetItem(i), &dt);
		}
		break;
	case DTN_DROPDOWN:
		{
//			NMHDR *nmhdr = (NMHDR*)lParam;
			if (this->showWeeknum)
			{
				HWND hwndCal = (HWND)SendMessage((HWND)this->hwnd, DTM_GETMONTHCAL, 0, 0);
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

void UI::GUIDateTimePicker::SetValue(NotNullPtr<Data::DateTime> dt)
{
	SYSTEMTIME t;
	Int8 tz = dt->GetTimeZoneQHR();
	dt->SetTimeZoneQHR(0);
	dt->ToSYSTEMTIME(&t);
	dt->SetTimeZoneQHR(tz);
	SendMessage((HWND)this->hwnd, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&t);
}

void UI::GUIDateTimePicker::SetValue(const Data::Timestamp &ts)
{
	SYSTEMTIME t;
	ts.ToSYSTEMTIME(&t);
	SendMessage((HWND)this->hwnd, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&t);
}

void UI::GUIDateTimePicker::GetSelectedTime(NotNullPtr<Data::DateTime> dt)
{
	SYSTEMTIME t;
	SendMessage((HWND)this->hwnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&t);
	Int8 tz = dt->GetTimeZoneQHR();
	dt->ToUTCTime();
	dt->SetValueSYSTEMTIME(&t);
	dt->SetTimeZoneQHR(tz);
}

Data::Timestamp UI::GUIDateTimePicker::GetSelectedTime()
{
	SYSTEMTIME t;
	SendMessage((HWND)this->hwnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&t);
	return Data::Timestamp(Data::DateTimeUtil::SYSTEMTIME2Ticks(&t), 0);
}

void UI::GUIDateTimePicker::SetFormat(const Char *format)
{
	const WChar *wptr = Text::StrToWCharNew((const UTF8Char*)format);
	SendMessage((HWND)this->hwnd, DTM_SETFORMATW, 0, (LPARAM)wptr);
	Text::StrDelNew(wptr);
}

void UI::GUIDateTimePicker::SetCalShowWeeknum(Bool showWeeknum)
{
	this->showWeeknum = showWeeknum;
}

void UI::GUIDateTimePicker::HandleDateChange(DateChangedHandler hdlr, void *obj)
{
	this->dateChangedHdlrs.Add(hdlr);
	this->dateChangedObjs.Add(obj);
}
