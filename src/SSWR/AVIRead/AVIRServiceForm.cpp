#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRServiceForm.h"

void __stdcall SSWR::AVIRead::AVIRServiceForm::OnServiceSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRServiceForm *me = (SSWR::AVIRead::AVIRServiceForm*)userObj;
	Text::String *s = me->lvService->GetSelectedItemTextNew();
	if (s)
	{
		IO::ServiceManager::ServiceDetail svc;
		if (me->svcMgr->ServiceGetDetail(s->ToCString(), &svc))
		{
			UTF8Char sbuff[64];
			UTF8Char *sptr;
			me->txtStatus->SetText(IO::ServiceInfo::RunStatusGetName(svc.status));
			Data::DateTime dt;
			dt.SetTicks(svc.startTimeTicks);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->txtStartTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, svc.procId);
			me->txtProcId->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, svc.memoryUsage);
			me->txtMemory->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->txtStatus->SetText(CSTR(""));
			me->txtStartTime->SetText(CSTR(""));
			me->txtProcId->SetText(CSTR(""));
			me->txtMemory->SetText(CSTR(""));
		}
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRServiceForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRServiceForm *me = (SSWR::AVIRead::AVIRServiceForm*)userObj;
	Text::String *s = me->lvService->GetSelectedItemTextNew();
	if (s)
	{
		if (me->svcMgr->ServiceStart(s->ToCString()))
		{
			OnServiceSelChg(me);
		}
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRServiceForm::OnStopClicked(void *userObj)
{
	SSWR::AVIRead::AVIRServiceForm *me = (SSWR::AVIRead::AVIRServiceForm*)userObj;
	Text::String *s = me->lvService->GetSelectedItemTextNew();
	if (s)
	{
		if (me->svcMgr->ServiceStop(s->ToCString()))
		{
			OnServiceSelChg(me);
		}
		s->Release();
	}
}

SSWR::AVIRead::AVIRServiceForm::AVIRServiceForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Service Info"));

	this->core = core;
	NEW_CLASS(this->svcMgr, IO::ServiceManager());

	NEW_CLASS(this->lvService, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvService->SetRect(0, 0, 220, 23, false);
	this->lvService->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lvService->AddColumn(CSTR("Name"), 150);
	this->lvService->AddColumn(CSTR("State"), 60);
	this->lvService->SetFullRowSelect(true);
	this->lvService->SetShowGrid(true);
	this->lvService->HandleSelChg(OnServiceSelChg, this);
	NEW_CLASS(this->hspService, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlCtrl, CSTR("Start")));
	this->btnStart->SetRect(4, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this->pnlCtrl, CSTR("Stop")));
	this->btnStop->SetRect(84, 4, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);
	NEW_CLASS(this->pnlDetail, UI::GUIPanel(ui, this));
	this->pnlDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this->pnlDetail, CSTR("Status")));
	this->lblStatus->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtStatus->SetReadOnly(true);
	this->txtStatus->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblProcId, UI::GUILabel(ui, this->pnlDetail, CSTR("Proc Id")));
	this->lblProcId->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtProcId, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtProcId->SetReadOnly(true);
	this->txtProcId->SetRect(104, 28, 150, 23, false);
	NEW_CLASS(this->lblStartTime, UI::GUILabel(ui, this->pnlDetail, CSTR("Start Time")));
	this->lblStartTime->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtStartTime, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtStartTime->SetReadOnly(true);
	this->txtStartTime->SetRect(104, 52, 150, 23, false);
	NEW_CLASS(this->lblMemory, UI::GUILabel(ui, this->pnlDetail, CSTR("Memory")));
	this->lblMemory->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtMemory, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtMemory->SetReadOnly(true);
	this->txtMemory->SetRect(104, 76, 150, 23, false);

	Data::ArrayList<IO::ServiceManager::ServiceItem*> svcList;
	UOSInt i;
	UOSInt j;
	IO::ServiceManager::ServiceItem *svc;
	this->svcMgr->QueryServiceList(&svcList);
	i = svcList.GetCount();
	while (i-- > 0)
	{
		svc = svcList.GetItem(i);
		j = this->lvService->AddItem(svc->name, 0);
		this->lvService->SetSubItem(j, 1, IO::ServiceInfo::ServiceStateGetName(svc->state));
	}
	this->svcMgr->FreeServiceList(&svcList);
}

SSWR::AVIRead::AVIRServiceForm::~AVIRServiceForm()
{
	DEL_CLASS(this->svcMgr);
}

void SSWR::AVIRead::AVIRServiceForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}