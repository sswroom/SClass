#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSetLocationSvcForm.h"
#include "Text/StringBuilder.h"

void __stdcall SSWR::AVIRead::AVIRSetLocationSvcForm::OnOKClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetLocationSvcForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetLocationSvcForm>();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRSetLocationSvcForm::OnCancelClick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSetLocationSvcForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSetLocationSvcForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRSetLocationSvcForm::OnGPSClick(AnyType userObj)
{
//	SSWR::AVIRead::AVIRSetLocationSvcForm *me = (SSWR::AVIRead::AVIRSetLocationSvcForm*)userObj;
}

SSWR::AVIRead::AVIRSetLocationSvcForm::AVIRSetLocationSvcForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 272, ui)
{
	this->SetText(CSTR("Set Location Service"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblGPS = ui->NewLabel(*this, CSTR("GPS Device"));
	this->lblGPS->SetRect(4, 4, 100, 23, false);
	this->chkGPS = ui->NewCheckBox(*this, CSTR("Enable"), false);
	this->chkGPS->SetRect(104, 4, 100, 23, false);
	this->btnGPS = ui->NewButton(*this, CSTR("Select"));
	this->btnGPS->SetRect(204, 4, 75, 23, false);
	this->btnGPS->HandleButtonClick(OnGPSClick, this);
	this->txtGPS = ui->NewTextBox(*this, CSTR(""));
	this->txtGPS->SetReadOnly(true);
	this->txtGPS->SetRect(284, 4, 120, 23, false);

	this->lblWifi = ui->NewLabel(*this, CSTR("WIFI"));
	this->lblWifi->SetRect(4, 28, 100, 23, false);
	this->chkWifi = ui->NewCheckBox(*this, CSTR("Enable"), false);
	this->chkWifi->SetRect(104, 28, 100, 23, false);


	this->btnOK = ui->NewButton(*this, CSTR("&OK"));
	this->btnOK->SetRect(104, 64, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClick, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(184, 64, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClick, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
}

SSWR::AVIRead::AVIRSetLocationSvcForm::~AVIRSetLocationSvcForm()
{
}

void SSWR::AVIRead::AVIRSetLocationSvcForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
