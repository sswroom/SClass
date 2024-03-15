#include "Stdafx.h"
#include "SSWR/AVIRead/AVIREmailAddrValidForm.h"

void __stdcall SSWR::AVIRead::AVIREmailAddrValidForm::OnValidateClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIREmailAddrValidForm> me = userObj.GetNN<SSWR::AVIRead::AVIREmailAddrValidForm>();
	Text::StringBuilderUTF8 sb;
	me->txtAddr->GetText(sb);
	Net::Email::EmailValidator::Status status = me->validator->Validate(sb.ToCString());
	me->txtStatus->SetText(Net::Email::EmailValidator::StatusGetName(status));
}

SSWR::AVIRead::AVIREmailAddrValidForm::AVIREmailAddrValidForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 240, ui)
{
	this->SetText(CSTR("Email Address Valid"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->validator, Net::Email::EmailValidator(this->core->GetSocketFactory(), this->core->GetLog()));

	this->lblAddr = ui->NewLabel(*this, CSTR("Email Address"));
	this->lblAddr->SetRect(4, 4, 100, 23, false);
	this->txtAddr = ui->NewTextBox(*this, CSTR(""));
	this->txtAddr->SetRect(104, 4, 200, 23, false);
	this->btnValidate = ui->NewButton(*this, CSTR("&Validate"));
	this->btnValidate->SetRect(104, 28, 75, 23, false);
	this->btnValidate->HandleButtonClick(OnValidateClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 52, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetReadOnly(true);
	this->txtStatus->SetRect(104, 52, 100, 23, false);
}

SSWR::AVIRead::AVIREmailAddrValidForm::~AVIREmailAddrValidForm()
{
	DEL_CLASS(this->validator);
}

void SSWR::AVIRead::AVIREmailAddrValidForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
