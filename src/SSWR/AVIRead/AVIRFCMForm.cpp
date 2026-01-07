#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Google/GoogleFCM.h"
#include "SSWR/AVIRead/AVIRFCMForm.h"

void __stdcall SSWR::AVIRead::AVIRFCMForm::OnSendClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRFCMForm> me = userObj.GetNN<SSWR::AVIRead::AVIRFCMForm>();
	Text::StringBuilderUTF8 sbAPIKey;
	Text::StringBuilderUTF8 sbDeviceToken;
	Text::StringBuilderUTF8 sbMessage;
	me->txtAPIKey->GetText(sbAPIKey);
	me->txtDeviceToken->GetText(sbDeviceToken);
	me->txtMessage->GetText(sbMessage);
	if (sbAPIKey.leng == 0)
	{
		me->txtStatus->SetText(CSTR("API Key is empty"));
		return;
	}
	if (sbDeviceToken.leng == 0)
	{
		me->txtStatus->SetText(CSTR("Device Token is empty"));
		return;
	}
	if (sbMessage.leng == 0)
	{
		me->txtStatus->SetText(CSTR("Message is empty"));
		return;
	}
	Text::StringBuilderUTF8 sbStatus;
	Net::Google::GoogleFCM fcm(me->core->GetTCPClientFactory(), me->ssl, sbAPIKey.ToCString());
	fcm.SendMessage(sbDeviceToken.ToCString(), sbMessage.ToCString(), sbStatus);
	me->txtStatus->SetText(sbStatus.ToCString());
}

SSWR::AVIRead::AVIRFCMForm::AVIRFCMForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 156, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Google FCM"));
	this->SetNoResize(true);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblAPIKey = ui->NewLabel(*this, CSTR("API Key"));
	this->lblAPIKey->SetRect(4, 4, 100, 23, false);
	this->txtAPIKey = ui->NewTextBox(*this, CSTR(""));
	this->txtAPIKey->SetRect(104, 4, 300, 23, false);
	this->lblDeviceToken = ui->NewLabel(*this, CSTR("Device Token"));
	this->lblDeviceToken->SetRect(4, 28, 100, 23, false);
	this->txtDeviceToken = ui->NewTextBox(*this, CSTR(""));
	this->txtDeviceToken->SetRect(104, 28, 600, 23, false);
	this->lblMessage = ui->NewLabel(*this, CSTR("Message"));
	this->lblMessage->SetRect(4, 52, 100, 23, false);
	this->txtMessage = ui->NewTextBox(*this, CSTR(""));
	this->txtMessage->SetRect(104, 52, 600, 23, false);
	this->btnSend = ui->NewButton(*this, CSTR("Send"));
	this->btnSend->SetRect(104, 76, 75, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Dest File"));
	this->lblStatus->SetRect(4, 100, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(104, 100, 300, 23, false);
	this->txtStatus->SetReadOnly(true);
}

SSWR::AVIRead::AVIRFCMForm::~AVIRFCMForm()
{
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRFCMForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
