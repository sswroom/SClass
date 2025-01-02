#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRRadioScanForm.h"

void __stdcall SSWR::AVIRead::AVIRRadioScanForm::OnWiFiClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRadioScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRadioScanForm>();
	me->ToggleWiFi();	
}

void SSWR::AVIRead::AVIRRadioScanForm::ToggleWiFi()
{

}

SSWR::AVIRead::AVIRRadioScanForm::AVIRRadioScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 156, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Radio Scan"));
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDashboard = this->tcMain->AddTabPage(CSTR("Dashboard"));
	this->lvDashboard = ui->NewListView(this->tpDashboard, UI::ListViewStyle::Table, 3);
	this->lvDashboard->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDashboard->SetFullRowSelect(true);
	this->lvDashboard->SetShowGrid(true);
	this->lvDashboard->AddColumn(CSTR("Type"), 80);
	this->lvDashboard->AddColumn(CSTR("Status"), 80);
	this->lvDashboard->AddColumn(CSTR("Count"), 100);

	this->tpWiFi = this->tcMain->AddTabPage(CSTR("WiFi"));
	this->pnlWiFi = ui->NewPanel(this->tpWiFi);
	this->pnlWiFi->SetRect(0, 0, 100, 31, false);
	this->pnlWiFi->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnWiFi = ui->NewButton(this->pnlWiFi, CSTR("Start"));
	this->btnWiFi->SetRect(4, 4, 75, 23, false);
	this->btnWiFi->HandleButtonClick(OnWiFiClicked, this);
	this->txtWiFiDetail = ui->NewTextBox(this->tpWiFi, CSTR(""), true);
	this->txtWiFiDetail->SetReadOnly(true);
	this->txtWiFiDetail->SetRect(0, 0, 100, 100, false);
	this->txtWiFiDetail->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvWiFi = ui->NewListView(this->tpWiFi, UI::ListViewStyle::Table, 8);
	this->lvWiFi->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvWiFi->SetShowGrid(true);
	this->lvWiFi->SetFullRowSelect(true);
	this->lvWiFi->AddColumn(CSTR("MAC Address"), 150);
	this->lvWiFi->AddColumn(CSTR("Frequency"), 100);
	this->lvWiFi->AddColumn(CSTR("RSSI"), 50);
	this->lvWiFi->AddColumn(CSTR("SSID"), 100);
	this->lvWiFi->AddColumn(CSTR("Vendor"), 150);
	this->lvWiFi->AddColumn(CSTR("Manufacturer"), 100);
	this->lvWiFi->AddColumn(CSTR("Model"), 100);
	this->lvWiFi->AddColumn(CSTR("S/N"), 100);
	this->lvDashboard->AddItem(CSTR("WiFi"), 0);
	this->lvDashboard->SetSubItem(0, 1, CSTR("Idle"));
	this->lvDashboard->SetSubItem(0, 2, CSTR("0"));
}

SSWR::AVIRead::AVIRRadioScanForm::~AVIRRadioScanForm()
{
}

void SSWR::AVIRead::AVIRRadioScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
