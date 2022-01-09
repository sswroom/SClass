#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganLocationForm.h"
#include "SSWR/OrganMgr/OrganTripForm.h"
#include "UI/MessageDialog.h"
#include "UI/GUICore.h"

void SSWR::OrganMgr::OrganTripForm::UpdateList()
{
	this->lbTrips->ClearItems();
	Trip *trip;
	Data::ArrayList<Trip*> *tripList = this->env->TripGetList();
	UTF8Char sbuff[256];
	UOSInt i;
	UOSInt j;
	this->updating = true;
	i = 0;
	j = tripList->GetCount();
	while (i < j)
	{
		trip = tripList->GetItem(i);
		trip->ToString(sbuff);
		this->lbTrips->AddItem(sbuff, trip);
		i++;
	}
	this->updating = false;
}

void __stdcall SSWR::OrganMgr::OrganTripForm::OnTripSelChg(void *userObj)
{
	OrganTripForm *me = (OrganTripForm*)userObj;
	Trip *trip = (Trip*)me->lbTrips->GetSelectedItem();
	if (trip && !me->updating)
	{
		Data::DateTime dt;
		dt.SetUnixTimestamp(trip->fromDate);
		dt.ToLocalTime();
		me->dtpFrom->SetValue(&dt);
		dt.SetUnixTimestamp(trip->toDate);
		dt.ToLocalTime();
		me->dtpTo->SetValue(&dt);
		Location *loc = me->env->LocationGet(trip->locId);
		if (loc)
		{
			me->txtLocation->SetText(loc->cname->v);
			me->locId = trip->locId;
		}
		else
		{
			me->txtLocation->SetText((const UTF8Char*)"");
			me->locId = 0;
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganTripForm::OnAddClicked(void *userObj)
{
	OrganTripForm *me = (OrganTripForm*)userObj;

	UTF8Char sbuff[256];
	Data::DateTime frDate;
	Data::DateTime toDate;
	Int64 ifrDate;
	Int64 itoDate;
	frDate.ToLocalTime();
	me->dtpFrom->GetSelectedTime(&frDate);
	toDate.ToLocalTime();
	me->dtpTo->GetSelectedTime(&toDate);
	ifrDate = frDate.ToUnixTimestamp();
	itoDate = toDate.ToUnixTimestamp();
	if (frDate > toDate)
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"TripFormErrorTime"), me->env->GetLang((const UTF8Char*)"TripFormTitle"), me);
		return;
	}
	if (me->locId <= 0)
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"TripFormErrorLocation"), me->env->GetLang((const UTF8Char*)"TripFormTitle"), me);
		return;
	}

	OSInt i = 0;
	OSInt j = (OSInt)me->lbTrips->GetCount() - 1;
	OSInt k;
	Trip *t;
	while (j >= i)
	{
		k = (i + j) >> 1;
		t = (Trip*)me->lbTrips->GetItem((UOSInt)k);
		if (t->fromDate <= itoDate && t->toDate >= ifrDate)
		{
			me->env->LocationGet(t->locId)->cname->ConcatTo(Text::StrConcatC(Text::StrConcat(sbuff, me->env->GetLang((const UTF8Char*)"TripFormErrorExist")), UTF8STRC(": ")));
			UI::MessageDialog::ShowDialog(sbuff, me->env->GetLang((const UTF8Char*)"TripFormTitle"), me);
			return;
		}
		if (t->fromDate > ifrDate)
		{
			j = k - 1;
		}
		else if (t->fromDate < ifrDate)
		{
			i = k + 1;
		}
		else
		{
			return;
		}
	}
	if (me->env->TripAdd(&frDate, &toDate, me->locId))
	{
		me->UpdateList();
	}
	else
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang((const UTF8Char*)"TripFormErrorUnk"), me->env->GetLang((const UTF8Char*)"TripFormTitle"), me);
		return;
	}
}

void __stdcall SSWR::OrganMgr::OrganTripForm::OnLocationClicked(void *userObj)
{
	OrganTripForm *me = (OrganTripForm*)userObj;
	
	OrganLocationForm *frm;
	NEW_CLASS(frm, OrganLocationForm(0, me->ui, me->env, OrganLocationForm::SM_CHILD, me->locId));
	if (frm->ShowDialog(me) == DR_OK)
	{
		Location *selVal = frm->GetSelVal();
		me->locId = selVal->id;
		me->txtLocation->SetText(selVal->cname->v);
	}
	DEL_CLASS(frm);
}

SSWR::OrganMgr::OrganTripForm::OrganTripForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env) : UI::GUIForm(parent, 640, 300, ui)
{
	this->SetFont(0, 0, 10.5, false);
	this->env = env;
	this->locId = 0;
	this->updating = false;

	this->SetText(this->env->GetLang((const UTF8Char*)"TripFormTitle"));

	NEW_CLASS(this->lbTrips, UI::GUIListBox(ui, this, false));
	this->lbTrips->SetRect(0, 0, 320, 268, false);
	this->lbTrips->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTrips->HandleSelectionChange(OnTripSelChg, this);
	NEW_CLASS(this->pnlDetail, UI::GUIPanel(ui, this));
	this->pnlDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblFrom, UI::GUILabel(ui, this->pnlDetail, this->env->GetLang((const UTF8Char*)"TripFormFrom")));
	this->lblFrom->SetRect(0, 24, 72, 23, false);
	NEW_CLASS(this->dtpFrom, UI::GUIDateTimePicker(ui, this->pnlDetail, UI::GUIDateTimePicker::ST_UPDOWN));
	this->dtpFrom->SetRect(72, 24, 176, 23, false);
	NEW_CLASS(this->lblTo, UI::GUILabel(ui, this->pnlDetail, this->env->GetLang((const UTF8Char*)"TripFormTo")));
	this->lblTo->SetRect(0, 56, 72, 23, false);
	NEW_CLASS(this->dtpTo, UI::GUIDateTimePicker(ui, this->pnlDetail, UI::GUIDateTimePicker::ST_UPDOWN));
	this->dtpTo->SetRect(72, 56, 176, 23, false);
	NEW_CLASS(this->lblLocation, UI::GUILabel(ui, this->pnlDetail, this->env->GetLang((const UTF8Char*)"TripFormLocation")));
	this->lblLocation->SetRect(0, 88, 72, 23, false);
	NEW_CLASS(this->txtLocation, UI::GUITextBox(ui, this->pnlDetail, (const UTF8Char*)""));
	this->txtLocation->SetRect(72, 88, 88, 23, false);
	this->txtLocation->SetReadOnly(true);
	NEW_CLASS(this->btnLocation, UI::GUIButton(ui, this->pnlDetail, this->env->GetLang((const UTF8Char*)"TripFormSelect")));
	this->btnLocation->SetRect(168, 88, 75, 23, false);
	this->btnLocation->HandleButtonClick(OnLocationClicked, this);
	NEW_CLASS(this->btnAdd, UI::GUIButton(ui, this->pnlDetail, this->env->GetLang((const UTF8Char*)"TripFormAdd")));
	this->btnAdd->SetRect(72, 128, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);

	this->UpdateList();
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganTripForm::~OrganTripForm()
{
}

void SSWR::OrganMgr::OrganTripForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::OrganMgr::OrganTripForm::SetTimes(Data::DateTime *fromTime, Data::DateTime *toTime)
{
	dtpFrom->SetValue(fromTime);
	dtpTo->SetValue(toTime);
}
