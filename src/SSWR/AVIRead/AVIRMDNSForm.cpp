#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRMDNSForm.h"

SSWR::AVIRead::AVIRMDNSForm::AVIRMDNSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("mDNS"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lbDev = ui->NewListBox(*this, false);
	this->lbDev->SetRect(0, 0, 150, 23, false);
	this->lbDev->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->hspDev = ui->NewHSplitter(*this, 3, false);
	this->lvServices = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvServices->SetRect(0, 0, 100, 100, false);
	this->lvServices->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvServices->SetShowGrid(true);
	this->lvServices->AddColumn(CSTR("Name"), 300);
	this->lvServices->AddColumn(CSTR("TTL"), 100);
	this->lvServices->AddColumn(CSTR("Class"), 100);
	this->vspServices = ui->NewVSplitter(*this, 3, false);
	this->txtServices = ui->NewTextBox(*this, CSTR(""), true);
	this->txtServices->SetReadOnly(true);
	this->txtServices->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASSNN(this->mdns, Net::MDNSClient(core->GetSocketFactory()));
	if (this->mdns->IsError())
	{
		ui->ShowMsgOK(CSTR("Error in initializing mDNS client"), CSTR("mDNS"), this);
	}
}

SSWR::AVIRead::AVIRMDNSForm::~AVIRMDNSForm()
{
	this->mdns.Delete();
}

void SSWR::AVIRead::AVIRMDNSForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
