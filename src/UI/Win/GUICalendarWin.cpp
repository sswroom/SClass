#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "UI/GUICalendar.h"
#include "UI/Win/WinCore.h"
#include <windows.h>
#include <Commctrl.h>

Int32 UI::GUICalendar::useCnt = 0;

UI::GUICalendar::GUICalendar(NotNullPtr<UI::GUICore> ui, Optional<UI::GUIClientControl> parent) : UI::GUIControl(ui, parent)
{
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
		INITCOMMONCONTROLSEX icex;
		icex.dwICC = ICC_DATE_CLASSES;
		InitCommonControlsEx(&icex);
	}

	Int32 style = WS_BORDER | WS_TABSTOP | WS_CHILD;
	NotNullPtr<UI::GUIClientControl> nnparent;
	if (parent.SetTo(nnparent) && nnparent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(NotNullPtr<UI::Win::WinCore>::ConvertFrom(ui)->GetHInst(), parent, MONTHCAL_CLASS, (const UTF8Char*)"", style, WS_EX_CLIENTEDGE, 0, 0, 200, 200);
}

UI::GUICalendar::~GUICalendar()
{
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
	}
}

Text::CStringNN UI::GUICalendar::GetObjectClass()
{
	return CSTR("Calendar");
}

OSInt UI::GUICalendar::OnNotify(UInt32 code, void *lParam)
{
	Data::DateTime dt;
	OSInt i;
	NMSELCHANGE *chg;
	switch (code)
	{
	case MCN_SELCHANGE:
		chg = (NMSELCHANGE *)lParam;
		dt.SetValueSYSTEMTIME(&chg->stSelStart);
		i = this->dateChangedHdlrs.GetCount();
		while (i-- > 0)
		{
			this->dateChangedHdlrs.GetItem(i)(this->dateChangedObjs.GetItem(i), &dt);
		}
	}
	return 0;
}

void UI::GUICalendar::GetSelectedTime(NotNullPtr<Data::DateTime> dt)
{
	SYSTEMTIME t;
	SendMessage((HWND)this->hwnd, MCM_GETCURSEL, 0, (LPARAM)&t);
	dt->SetValueSYSTEMTIME(&t);
}

void UI::GUICalendar::HandleDateChange(DateChangedHandler hdlr, void *obj)
{
	this->dateChangedHdlrs.Add(hdlr);
	this->dateChangedObjs.Add(obj);
}
