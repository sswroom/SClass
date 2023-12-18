#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRSetLocationSvcForm.h"
#include "Text/StringBuilder.h"

void __stdcall SSWR::AVIRead::AVIRSetLocationSvcForm::OnOKClick(void *userObj)
{
	SSWR::AVIRead::AVIRSetLocationSvcForm *me = (SSWR::AVIRead::AVIRSetLocationSvcForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRSetLocationSvcForm::OnCancelClick(void *userObj)
{
	SSWR::AVIRead::AVIRSetLocationSvcForm *me = (SSWR::AVIRead::AVIRSetLocationSvcForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRSetLocationSvcForm::OnGPSClick(void *userObj)
{
//	SSWR::AVIRead::AVIRSetLocationSvcForm *me = (SSWR::AVIRead::AVIRSetLocationSvcForm*)userObj;
}

SSWR::AVIRead::AVIRSetLocationSvcForm::AVIRSetLocationSvcForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 272, ui)
{
	this->SetText(CSTR("Set Location Service"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblGPS, UI::GUILabel(ui, *this, CSTR("GPS Device")));
	this->lblGPS->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->chkGPS, UI::GUICheckBox(ui, *this, CSTR("Enable"), false));
	this->chkGPS->SetRect(104, 4, 100, 23, false);
	this->btnGPS = ui->NewButton(*this, CSTR("Select"));
	this->btnGPS->SetRect(204, 4, 75, 23, false);
	this->btnGPS->HandleButtonClick(OnGPSClick, this);
	NEW_CLASS(this->txtGPS, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtGPS->SetReadOnly(true);
	this->txtGPS->SetRect(284, 4, 120, 23, false);

	NEW_CLASS(this->lblWifi, UI::GUILabel(ui, *this, CSTR("WIFI")));
	this->lblWifi->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->chkWifi, UI::GUICheckBox(ui, *this, CSTR("Enable"), false));
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
