#include "Stdafx.h"
#include "IO/Registry.h"
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
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	this->updating = true;
	i = 0;
	j = tripList->GetCount();
	while (i < j)
	{
		trip = tripList->GetItem(i);
		sptr = trip->ToString(sbuff);
		this->lbTrips->AddItem(CSTRP(sbuff, sptr), trip);
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
			me->txtLocation->SetText(loc->cname->ToCString());
			me->locId = trip->locId;
		}
		else
		{
			me->txtLocation->SetText(CSTR(""));
			me->locId = 0;
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganTripForm::OnAddClicked(void *userObj)
{
	OrganTripForm *me = (OrganTripForm*)userObj;

	UTF8Char sbuff[256];
	UTF8Char *sptr;
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
		UI::MessageDialog::ShowDialog(me->env->GetLang(UTF8STRC("TripFormErrorTime")), me->env->GetLang(UTF8STRC("TripFormTitle")), me);
		return;
	}
	if (me->locId <= 0)
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang(UTF8STRC("TripFormErrorLocation")), me->env->GetLang(UTF8STRC("TripFormTitle")), me);
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
			sptr = me->env->LocationGet(t->locId)->cname->ConcatTo(Text::StrConcatC(me->env->GetLang(UTF8STRC("TripFormErrorExist")).ConcatTo(sbuff), UTF8STRC(": ")));
			UI::MessageDialog::ShowDialog(CSTRP(sbuff, sptr), me->env->GetLang(UTF8STRC("TripFormTitle")), me);
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
		UI::MessageDialog::ShowDialog(me->env->GetLang(UTF8STRC("TripFormErrorUnk")), me->env->GetLang(UTF8STRC("TripFormTitle")), me);
		return;
	}
}

void __stdcall SSWR::OrganMgr::OrganTripForm::OnLocationClicked(void *userObj)
{
	OrganTripForm *me = (OrganTripForm*)userObj;
	
	OrganLocationForm frm(0, me->ui, me->env, OrganLocationForm::SM_CHILD, me->locId);
	if (frm.ShowDialog(me) == DR_OK)
	{
		Location *selVal = frm.GetSelVal();
		me->locId = selVal->id;
		me->txtLocation->SetText(selVal->cname->ToCString());
		IO::Registry *reg = IO::Registry::OpenLocalSoftware(L"OrganMgr");
		if (reg)
		{
			reg->SetValue(L"TripLocationLast", me->locId);
			IO::Registry::CloseRegistry(reg);
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganTripForm::OnDate1HrClicked(void *userObj)
{
	OrganTripForm *me = (OrganTripForm*)userObj;
	if (me->refTime.ticks != 0)
	{
		Data::DateTime dt(me->refTime.ticks, me->refTime.tzQhr);
		UInt8 min = dt.GetMinute();
		if (min < 15)
		{
			dt.SetValue(dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour() - 1, 30, 0, 0, dt.GetTimeZoneQHR());
			me->dtpFrom->SetValue(&dt);
			dt.AddHour(1);
			me->dtpTo->SetValue(&dt);
		}
		else if (min < 45)
		{
			dt.SetValue(dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), 0, 0, 0, dt.GetTimeZoneQHR());
			me->dtpFrom->SetValue(&dt);
			dt.AddHour(1);
			me->dtpTo->SetValue(&dt);
		}
		else
		{
			dt.SetValue(dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), 30, 0, 0, dt.GetTimeZoneQHR());
			me->dtpFrom->SetValue(&dt);
			dt.AddHour(1);
			me->dtpTo->SetValue(&dt);
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganTripForm::OnLocationLastClicked(void *userObj)
{
	OrganTripForm *me = (OrganTripForm*)userObj;
	IO::Registry *reg = IO::Registry::OpenLocalSoftware(L"OrganMgr");
	if (reg)
	{
		Int32 locId = reg->GetValueI32(L"TripLocationLast");
		IO::Registry::CloseRegistry(reg);
		if (locId != 0)
		{
			SSWR::OrganMgr::Location *loc = me->env->LocationGet(locId);
			me->locId = loc->id;
			me->txtLocation->SetText(loc->cname->ToCString());
		}
	}
}

SSWR::OrganMgr::OrganTripForm::OrganTripForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env) : UI::GUIForm(parent, 640, 300, ui)
{
	this->SetFont(0, 0, 10.5, false);
	this->env = env;
	this->locId = 0;
	this->updating = false;
	this->refTime = Data::Timestamp(0, 0);

	this->SetText(this->env->GetLang(UTF8STRC("TripFormTitle")));

	NEW_CLASS(this->lbTrips, UI::GUIListBox(ui, this, false));
	this->lbTrips->SetRect(0, 0, 320, 268, false);
	this->lbTrips->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTrips->HandleSelectionChange(OnTripSelChg, this);
	NEW_CLASS(this->pnlDetail, UI::GUIPanel(ui, this));
	this->pnlDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblFrom, UI::GUILabel(ui, this->pnlDetail, this->env->GetLang(UTF8STRC("TripFormFrom"))));
	this->lblFrom->SetRect(0, 24, 72, 23, false);
	NEW_CLASS(this->dtpFrom, UI::GUIDateTimePicker(ui, this->pnlDetail, UI::GUIDateTimePicker::ST_UPDOWN));
	this->dtpFrom->SetRect(72, 24, 176, 23, false);
	NEW_CLASS(this->lblTo, UI::GUILabel(ui, this->pnlDetail, this->env->GetLang(UTF8STRC("TripFormTo"))));
	this->lblTo->SetRect(0, 56, 72, 23, false);
	NEW_CLASS(this->dtpTo, UI::GUIDateTimePicker(ui, this->pnlDetail, UI::GUIDateTimePicker::ST_UPDOWN));
	this->dtpTo->SetRect(72, 56, 176, 23, false);
	NEW_CLASS(this->btnDate1Hr, UI::GUIButton(ui, this->pnlDetail, this->env->GetLang(UTF8STRC("TripForm1Hr"))));
	this->btnDate1Hr->SetRect(248, 56, 75, 23, false);
	this->btnDate1Hr->HandleButtonClick(OnDate1HrClicked, this);
	NEW_CLASS(this->lblLocation, UI::GUILabel(ui, this->pnlDetail, this->env->GetLang(UTF8STRC("TripFormLocation"))));
	this->lblLocation->SetRect(0, 88, 72, 23, false);
	NEW_CLASS(this->txtLocation, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtLocation->SetRect(72, 88, 88, 23, false);
	this->txtLocation->SetReadOnly(true);
	NEW_CLASS(this->btnLocation, UI::GUIButton(ui, this->pnlDetail, this->env->GetLang(UTF8STRC("TripFormSelect"))));
	this->btnLocation->SetRect(168, 88, 75, 23, false);
	this->btnLocation->HandleButtonClick(OnLocationClicked, this);
	NEW_CLASS(this->btnLocationLast, UI::GUIButton(ui, this->pnlDetail, this->env->GetLang(UTF8STRC("TripFormLastLoc"))));
	this->btnLocationLast->SetRect(248, 88, 75, 23, false);
	this->btnLocationLast->HandleButtonClick(OnLocationLastClicked, this);
	NEW_CLASS(this->btnAdd, UI::GUIButton(ui, this->pnlDetail, this->env->GetLang(UTF8STRC("TripFormAdd"))));
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

void SSWR::OrganMgr::OrganTripForm::SetTimes(Data::DateTime *refTime, Data::DateTime *fromTime, Data::DateTime *toTime)
{
	this->refTime = Data::Timestamp(refTime->ToTicks(), refTime->GetTimeZoneQHR());
	dtpFrom->SetValue(fromTime);
	dtpTo->SetValue(toTime);
}
