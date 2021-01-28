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

UI::GUIDateTimePicker::GUIDateTimePicker(GUICore *ui, UI::GUIClientControl *parent, SelectType st) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->dateChangedHdlrs, Data::ArrayList<DateChangedHandler>());
	NEW_CLASS(this->dateChangedObjs, Data::ArrayList<void*>());

	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		INITCOMMONCONTROLSEX icex;
		icex.dwSize = sizeof(icex);
		icex.dwICC = ICC_DATE_CLASSES;
		InitCommonControlsEx(&icex);
	}
	this->showWeeknum = false;

	Int32 style = WS_BORDER | WS_TABSTOP | WS_CHILD;
	if (st == ST_UPDOWN)
	{
		style = style | DTS_UPDOWN;
	}
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((GUICoreWin*)ui)->GetHInst(), parent, DATETIMEPICK_CLASS, (const UTF8Char*)"", style, WS_EX_CLIENTEDGE, 0, 0, 200, 200);
	this->SetFormat("yyyy-MM-dd HH:mm:ss");
}

UI::GUIDateTimePicker::~GUIDateTimePicker()
{
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
	}
	DEL_CLASS(this->dateChangedHdlrs);
	DEL_CLASS(this->dateChangedObjs);
}

const UTF8Char *UI::GUIDateTimePicker::GetObjectClass()
{
	return (const UTF8Char*)"DateTimePicker";
}

OSInt UI::GUIDateTimePicker::OnNotify(Int32 code, void *lParam)
{
	Data::DateTime dt;
	OSInt i;
	LPNMDATETIMECHANGE chg;

	switch (code)
	{
	case DTN_DATETIMECHANGE:
		chg = (LPNMDATETIMECHANGE)lParam;
		dt.SetValueSYSTEMTIME(&chg->st);
		i = this->dateChangedHdlrs->GetCount();
		while (i-- > 0)
		{
			this->dateChangedHdlrs->GetItem(i)(this->dateChangedObjs->GetItem(i), &dt);
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
				UInt32 style = GetWindowLong(hwndCal, GWL_STYLE);
				style = style | MCS_WEEKNUMBERS;
				SetWindowLong(hwndCal, GWL_STYLE, style);
				GetWindowRect(hwndCal, &rcCal);
				MoveWindow(hwndCal, rcCal.left, rcCal.top, rcCal.right - rcCal.left + 30, rcCal.bottom - rcCal.top, FALSE);
			}
		}
		break;
	}
	return 0;
}

void UI::GUIDateTimePicker::SetValue(Data::DateTime *dt)
{
	SYSTEMTIME t;
	Int32 tz = dt->GetTimeZoneQHR();
	dt->SetTimeZoneQHR(0);
	dt->ToSYSTEMTIME(&t);
	dt->SetTimeZoneQHR(tz);
	SendMessage((HWND)this->hwnd, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&t);
}

void UI::GUIDateTimePicker::GetSelectedTime(Data::DateTime *dt)
{
	SYSTEMTIME t;
	SendMessage((HWND)this->hwnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&t);
	Int32 tz = dt->GetTimeZoneQHR();
	dt->ToUTCTime();
	dt->SetValueSYSTEMTIME(&t);
	dt->SetTimeZoneQHR(tz);
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
	this->dateChangedHdlrs->Add(hdlr);
	this->dateChangedObjs->Add(obj);
}
