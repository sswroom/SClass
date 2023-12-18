#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "SSWR/AVIRead/AVIRServiceCreateForm.h"
#include "SSWR/AVIRead/AVIRServiceForm.h"

void __stdcall SSWR::AVIRead::AVIRServiceForm::OnServiceSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRServiceForm *me = (SSWR::AVIRead::AVIRServiceForm*)userObj;
	Text::String *s = me->lvService->GetSelectedItemTextNew();
	if (s)
	{
		me->txtName->SetText(s->ToCString());
		IO::ServiceManager::ServiceDetail svc;
		if (me->svcMgr.ServiceGetDetail(s->ToCString(), &svc))
		{
			UTF8Char sbuff[64];
			UTF8Char *sptr;
			me->txtStatus->SetText(IO::ServiceInfo::RunStatusGetName(svc.status));
			me->txtEnabled->SetText(IO::ServiceInfo::ServiceStateGetName(svc.enabled));
			sptr = svc.startTime.ToStringNoZone(sbuff);
			me->txtStartTime->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, svc.procId);
			me->txtProcId->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, svc.memoryUsage);
			me->txtMemory->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->txtStatus->SetText(CSTR(""));
			me->txtEnabled->SetText(CSTR(""));
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
		if (me->svcMgr.ServiceStart(s->ToCString()))
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
		if (me->svcMgr.ServiceStop(s->ToCString()))
		{
			OnServiceSelChg(me);
		}
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRServiceForm::OnEnableClicked(void *userObj)
{
	SSWR::AVIRead::AVIRServiceForm *me = (SSWR::AVIRead::AVIRServiceForm*)userObj;
	Text::String *s = me->lvService->GetSelectedItemTextNew();
	if (s)
	{
		if (me->svcMgr.ServiceEnable(s->ToCString()))
		{
			OnServiceSelChg(me);
		}
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRServiceForm::OnDisableClicked(void *userObj)
{
	SSWR::AVIRead::AVIRServiceForm *me = (SSWR::AVIRead::AVIRServiceForm*)userObj;
	Text::String *s = me->lvService->GetSelectedItemTextNew();
	if (s)
	{
		if (me->svcMgr.ServiceDisable(s->ToCString()))
		{
			OnServiceSelChg(me);
		}
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRServiceForm::OnDeleteClicked(void *userObj)
{
	SSWR::AVIRead::AVIRServiceForm *me = (SSWR::AVIRead::AVIRServiceForm*)userObj;
	Text::String *s = me->lvService->GetSelectedItemTextNew();
	if (s)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Are you sure to delete service \""));
		sb.Append(s);
		sb.AppendC(UTF8STRC("\"?"));
		if (me->ui->ShowMsgYesNo(sb.ToCString(), CSTR("Service"), me))
		{
			if (!me->svcMgr.ServiceDelete(s->ToCString()))
			{
				me->ui->ShowMsgOK(CSTR("Error in deleting service"), CSTR("Service"), me);
			}
			else
			{
				me->lvService->ClearItems();
				me->UpdateSvcList();
			}
		}
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRServiceForm::OnCreateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRServiceForm *me = (SSWR::AVIRead::AVIRServiceForm*)userObj;
	SSWR::AVIRead::AVIRServiceCreateForm frm(0, me->ui, me->core);
	if (frm.ShowDialog(me))
	{
		me->lvService->ClearItems();
		me->UpdateSvcList();
	}
}

void SSWR::AVIRead::AVIRServiceForm::UpdateSvcList()
{
	Data::ArrayList<IO::ServiceManager::ServiceItem*> svcList;
	UOSInt i;
	UOSInt j;
	IO::ServiceManager::ServiceItem *svc;
	this->svcMgr.QueryServiceList(&svcList);
	IO::ServiceManager::ServiceComparator comparator;
	Data::Sort::ArtificialQuickSort::Sort(&svcList, &comparator);
	i = 0;
	j = svcList.GetCount();
	while (i < j)
	{
		svc = svcList.GetItem(i);
		this->lvService->AddItem(svc->name, 0);
		if (svc->state == IO::ServiceInfo::ServiceState::Unknown)
		{
			this->lvService->SetSubItem(i, 1, IO::ServiceInfo::RunStatusGetName(svc->runStatus));
		}
		else
		{
			this->lvService->SetSubItem(i, 1, IO::ServiceInfo::ServiceStateGetName(svc->state));
		}
		i++;
	}
	this->svcMgr.FreeServiceList(&svcList);
}

SSWR::AVIRead::AVIRServiceForm::AVIRServiceForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Service Info"));

	this->core = core;

	NEW_CLASS(this->lvService, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvService->SetRect(0, 0, 220, 23, false);
	this->lvService->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lvService->AddColumn(CSTR("Name"), 150);
	this->lvService->AddColumn(CSTR("State"), 60);
	this->lvService->SetFullRowSelect(true);
	this->lvService->SetShowGrid(true);
	this->lvService->HandleSelChg(OnServiceSelChg, this);
	this->hspService = ui->NewHSplitter(*this, 3, false);
	NEW_CLASSNN(this->pnlCtrl, UI::GUIPanel(ui, *this));
	this->pnlCtrl->SetRect(0, 0, 100, 55, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnStart = ui->NewButton(this->pnlCtrl, CSTR("Start"));
	this->btnStart->SetRect(4, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->btnStop = ui->NewButton(this->pnlCtrl, CSTR("Stop"));
	this->btnStop->SetRect(84, 4, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);
	this->btnEnable = ui->NewButton(this->pnlCtrl, CSTR("Enable"));
	this->btnEnable->SetRect(164, 4, 75, 23, false);
	this->btnEnable->HandleButtonClick(OnEnableClicked, this);
	this->btnDisable = ui->NewButton(this->pnlCtrl, CSTR("Disable"));
	this->btnDisable->SetRect(244, 4, 75, 23, false);
	this->btnDisable->HandleButtonClick(OnDisableClicked, this);
	this->btnDelete = ui->NewButton(this->pnlCtrl, CSTR("Delete"));
	this->btnDelete->SetRect(4, 28, 75, 23, false);
	this->btnDelete->HandleButtonClick(OnDeleteClicked, this);
	this->btnCreate = ui->NewButton(this->pnlCtrl, CSTR("Create"));
	this->btnCreate->SetRect(84, 28, 75, 23, false);
	this->btnCreate->HandleButtonClick(OnCreateClicked, this);
	NEW_CLASSNN(this->pnlDetail, UI::GUIPanel(ui, *this));
	this->pnlDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this->pnlDetail, CSTR("Name")));
	this->lblName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtName, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtName->SetReadOnly(true);
	this->txtName->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this->pnlDetail, CSTR("Status")));
	this->lblStatus->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtStatus->SetReadOnly(true);
	this->txtStatus->SetRect(104, 28, 150, 23, false);
	NEW_CLASS(this->lblEnabled, UI::GUILabel(ui, this->pnlDetail, CSTR("Enabled")));
	this->lblEnabled->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtEnabled, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtEnabled->SetReadOnly(true);
	this->txtEnabled->SetRect(104, 52, 150, 23, false);
	NEW_CLASS(this->lblProcId, UI::GUILabel(ui, this->pnlDetail, CSTR("Proc Id")));
	this->lblProcId->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtProcId, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtProcId->SetReadOnly(true);
	this->txtProcId->SetRect(104, 76, 150, 23, false);
	NEW_CLASS(this->lblStartTime, UI::GUILabel(ui, this->pnlDetail, CSTR("Start Time")));
	this->lblStartTime->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtStartTime, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtStartTime->SetReadOnly(true);
	this->txtStartTime->SetRect(104, 100, 150, 23, false);
	NEW_CLASS(this->lblMemory, UI::GUILabel(ui, this->pnlDetail, CSTR("Memory")));
	this->lblMemory->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtMemory, UI::GUITextBox(ui, this->pnlDetail, CSTR("")));
	this->txtMemory->SetReadOnly(true);
	this->txtMemory->SetRect(104, 124, 150, 23, false);
	this->UpdateSvcList();
}

SSWR::AVIRead::AVIRServiceForm::~AVIRServiceForm()
{
}

void SSWR::AVIRead::AVIRServiceForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
