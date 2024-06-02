#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRVBoxManagerForm.h"

void __stdcall SSWR::AVIRead::AVIRVBoxManagerForm::OnVMSSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRVBoxManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRVBoxManagerForm>();
	me->UpdateVMInfo();
}

void SSWR::AVIRead::AVIRVBoxManagerForm::UpdateVMInfo()
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = this->lbVMS->GetSelectedIndex();
	NN<IO::VBoxManager::VMId> vm;
	NN<IO::VBoxVMInfo> info;
	if (this->lbVMS->GetItem(i).GetOpt<IO::VBoxManager::VMId>().SetTo(vm))
	{
		Bool needUpdate = vm->name->Equals(UTF8STRC("<inaccessible>"));
		if (this->vbox.GetVMInfo(vm).SetTo(info))
		{
			this->txtState->SetText(IO::VBoxVMInfo::StateGetName(info->GetState()));
			sptr = info->GetStateSince().ToLocalTime().ToStringNoZone(sbuff);
			this->txtStateSince->SetText(CSTRP(sbuff, sptr));
			info.Delete();
			if (needUpdate)
			{
				this->lbVMS->SetItemText(i, vm->name->ToCString());
			}
		}
		else
		{
			this->txtState->SetText(CSTR("-"));
			this->txtStateSince->SetText(CSTR("-"));
		}
	}
	else
	{
		this->txtState->SetText(CSTR("-"));
		this->txtStateSince->SetText(CSTR("-"));
	}
}

SSWR::AVIRead::AVIRVBoxManagerForm::AVIRVBoxManagerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("VirtualBox Manager"));
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlVersion = ui->NewPanel(*this);
	this->pnlVersion->SetRect(0, 0, 100, 31, false);
	this->pnlVersion->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblVersion = ui->NewLabel(this->pnlVersion, CSTR("Version"));
	this->lblVersion->SetRect(4, 4, 100, 23, false);
	this->txtVersion = ui->NewTextBox(this->pnlVersion, CSTR(""));
	this->txtVersion->SetReadOnly(true);
	this->txtVersion->SetRect(104, 4, 200, 23, false);
	this->lbVMS = ui->NewListBox(*this, false);
	this->lbVMS->SetRect(0, 0, 150, 23, false);
	this->lbVMS->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbVMS->HandleSelectionChange(OnVMSSelChg, this);
	this->hspVM = ui->NewHSplitter(*this, 3, false);
	this->tcVM = ui->NewTabControl(*this);
	this->tcVM->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpControl = this->tcVM->AddTabPage(CSTR("Control"));
	this->lblState = ui->NewLabel(this->tpControl, CSTR("State"));
	this->lblState->SetRect(4, 4, 100, 23, false);
	this->txtState = ui->NewTextBox(this->tpControl, CSTR(""));
	this->txtState->SetRect(104, 4, 150, 23, false);
	this->txtState->SetReadOnly(true);
	this->lblStateSince = ui->NewLabel(this->tpControl, CSTR("Since"));
	this->lblStateSince->SetRect(4, 28, 100, 23, false);
	this->txtStateSince = ui->NewTextBox(this->tpControl, CSTR(""));
	this->txtStateSince->SetRect(104, 28, 150, 23, false);
	this->txtStateSince->SetReadOnly(true);

	Text::String *version = this->vbox.GetVersion();
	if (version)
	{
		this->txtVersion->SetText(version->ToCString());
	}
	NN<const Data::ArrayListNN<IO::VBoxManager::VMId>> vms = this->vbox.GetVMS();
	NN<IO::VBoxManager::VMId> vm;
	UOSInt i = 0;
	UOSInt j = vms->GetCount();
	while (i < j)
	{
		vm = vms->GetItemNoCheck(i);
		this->lbVMS->AddItem(vm->name, vm);
		i++;
	}
}

SSWR::AVIRead::AVIRVBoxManagerForm::~AVIRVBoxManagerForm()
{
}

void SSWR::AVIRead::AVIRVBoxManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
