#include "Stdafx.h"
#include "SSWR/AVIRead/AVIREmailAddrValidForm.h"

void __stdcall SSWR::AVIRead::AVIREmailAddrValidForm::OnValidateClicked(void *userObj)
{
	SSWR::AVIRead::AVIREmailAddrValidForm *me = (SSWR::AVIRead::AVIREmailAddrValidForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtAddr->GetText(&sb);
	Net::Email::EmailValidator::Status status = me->validator->Validate(sb.ToString());
	me->txtStatus->SetText(Net::Email::EmailValidator::StatusGetName(status));
}

SSWR::AVIRead::AVIREmailAddrValidForm::AVIREmailAddrValidForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 240, ui)
{
	this->SetText((const UTF8Char*)"Email Address Valid");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->validator, Net::Email::EmailValidator(this->core->GetSocketFactory()));

	NEW_CLASS(this->lblAddr, UI::GUILabel(ui, this, (const UTF8Char*)"Email Address"));
	this->lblAddr->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAddr, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtAddr->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->btnValidate, UI::GUIButton(ui, this, (const UTF8Char*)"&Validate"));
	this->btnValidate->SetRect(104, 28, 75, 23, false);
	this->btnValidate->HandleButtonClick(OnValidateClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, (const UTF8Char*)"Status"));
	this->lblStatus->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, (const UTF8Char*)""));
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
