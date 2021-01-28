#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Interlocked.h"
#include "UI/GUICalendar.h"
#include "UI/GUICoreWin.h"
#include <windows.h>
#include <Commctrl.h>

Int32 UI::GUICalendar::useCnt = 0;

UI::GUICalendar::GUICalendar(UI::GUICore *ui, UI::GUIClientControl *parent) : UI::GUIControl(ui, parent)
{
	NEW_CLASS(this->dateChangedHdlrs, Data::ArrayList<DateChangedHandler>());
	NEW_CLASS(this->dateChangedObjs, Data::ArrayList<void*>());

	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
		INITCOMMONCONTROLSEX icex;
		icex.dwICC = ICC_DATE_CLASSES;
		InitCommonControlsEx(&icex);
	}

	Int32 style = WS_BORDER | WS_TABSTOP | WS_CHILD;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)ui)->GetHInst(), parent, MONTHCAL_CLASS, (const UTF8Char*)"", style, WS_EX_CLIENTEDGE, 0, 0, 200, 200);
}

UI::GUICalendar::~GUICalendar()
{
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
	}
	DEL_CLASS(this->dateChangedHdlrs);
	DEL_CLASS(this->dateChangedObjs);
}

const UTF8Char *UI::GUICalendar::GetObjectClass()
{
	return (const UTF8Char*)"Calendar";
}

OSInt UI::GUICalendar::OnNotify(Int32 code, void *lParam)
{
	Data::DateTime dt;
	OSInt i;
	NMSELCHANGE *chg;
	switch (code)
	{
	case MCN_SELCHANGE:
		chg = (NMSELCHANGE *)lParam;
		dt.SetValueSYSTEMTIME(&chg->stSelStart);
		i = this->dateChangedHdlrs->GetCount();
		while (i-- > 0)
		{
			this->dateChangedHdlrs->GetItem(i)(this->dateChangedObjs->GetItem(i), &dt);
		}
	}
	return 0;
}

void UI::GUICalendar::GetSelectedTime(Data::DateTime *dt)
{
	SYSTEMTIME t;
	SendMessage((HWND)this->hwnd, MCM_GETCURSEL, 0, (LPARAM)&t);
	dt->SetValueSYSTEMTIME(&t);
}

void UI::GUICalendar::HandleDateChange(DateChangedHandler hdlr, void *obj)
{
	this->dateChangedHdlrs->Add(hdlr);
	this->dateChangedObjs->Add(obj);
}
