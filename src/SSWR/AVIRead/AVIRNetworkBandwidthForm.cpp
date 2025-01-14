#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRNetworkBandwidthForm.h"

SSWR::AVIRead::AVIRNetworkBandwidthForm::AVIRNetworkBandwidthForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 300, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Network Bandwidth"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblIP = ui->NewLabel(*this, CSTR("IP"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	this->cboIP = ui->NewComboBox(*this, false);
	this->cboIP->SetRect(104, 4, 150, 23, false);
	this->lblType = ui->NewLabel(*this, CSTR("Type"));
	this->lblType->SetRect(4, 28, 100, 23, false);
	this->cboType = ui->NewComboBox(*this, false);
	this->cboType->SetRect(104, 28, 150, 23, false);
	this->lblFormat = ui->NewLabel(*this, CSTR("Format"));
	this->lblFormat->SetRect(4, 52, 100, 23, false);
	this->cboFormat = ui->NewComboBox(*this, false);
	this->cboFormat->SetRect(104, 52, 150, 23, false);
	this->lblFileName = ui->NewLabel(*this, CSTR("File Name"));
	this->lblFileName->SetRect(4, 76, 100, 23, false);
	this->txtFileName = ui->NewTextBox(*this, CSTR(""));
	this->txtFileName->SetRect(104, 76, 500, 23, false);
	this->btnAutoGen = ui->NewButton(*this, CSTR("Auto Gen"));
	this->btnAutoGen->SetRect(604, 76, 75, 23, false);
	this->btnAutoGen->HandleButtonClick(OnAutoGenClicked, this);
	this->btnBrowse = ui->NewButton(*this, CSTR("&Browse"));
	this->btnBrowse->SetRect(684, 76, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	this->btnStart = ui->NewButton(*this, CSTR("Start"));
	this->btnStart->SetRect(104, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblPacketCnt = ui->NewLabel(*this, CSTR("Packet Count"));
	this->lblPacketCnt->SetRect(4, 124, 100, 23, false);
	this->txtPacketCnt = ui->NewTextBox(*this, CSTR(""));
	this->txtPacketCnt->SetReadOnly(true);
	this->txtPacketCnt->SetRect(104, 124, 150, 23, false);
	this->lblDataSize = ui->NewLabel(*this, CSTR("Data Size"));
	this->lblDataSize->SetRect(4, 148, 100, 23, false);
	this->txtDataSize = ui->NewTextBox(*this, CSTR(""));
	this->txtDataSize->SetReadOnly(true);
	this->txtDataSize->SetRect(104, 148, 150, 23, false);

}

SSWR::AVIRead::AVIRNetworkBandwidthForm::~AVIRNetworkBandwidthForm()
{
	SDEL_CLASS(this->capture);
}

void SSWR::AVIRead::AVIRNetworkBandwidthForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
