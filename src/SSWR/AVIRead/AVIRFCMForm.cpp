#include "Stdafx.h"
#include "Net/GoogleFCM.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRFCMForm.h"

void __stdcall SSWR::AVIRead::AVIRFCMForm::OnSendClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFCMForm *me = (SSWR::AVIRead::AVIRFCMForm*)userObj;
	Text::StringBuilderUTF8 sbAPIKey;
	Text::StringBuilderUTF8 sbDeviceToken;
	Text::StringBuilderUTF8 sbMessage;
	me->txtAPIKey->GetText(&sbAPIKey);
	me->txtDeviceToken->GetText(&sbDeviceToken);
	me->txtMessage->GetText(&sbMessage);
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
	Net::GoogleFCM::SendMessage(me->core->GetSocketFactory(), me->ssl, sbAPIKey.ToCString(), sbDeviceToken.ToCString(), sbMessage.ToCString(), &sbStatus);
	me->txtStatus->SetText(sbStatus.ToCString());
}

SSWR::AVIRead::AVIRFCMForm::AVIRFCMForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 156, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Google FCM"));
	this->SetNoResize(true);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblAPIKey, UI::GUILabel(ui, this, CSTR("API Key")));
	this->lblAPIKey->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAPIKey, UI::GUITextBox(ui, this, CSTR("")));
	this->txtAPIKey->SetRect(104, 4, 300, 23, false);
	NEW_CLASS(this->lblDeviceToken, UI::GUILabel(ui, this, CSTR("Device Token")));
	this->lblDeviceToken->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDeviceToken, UI::GUITextBox(ui, this, CSTR("")));
	this->txtDeviceToken->SetRect(104, 28, 600, 23, false);
	NEW_CLASS(this->lblMessage, UI::GUILabel(ui, this, CSTR("Message")));
	this->lblMessage->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtMessage, UI::GUITextBox(ui, this, CSTR("")));
	this->txtMessage->SetRect(104, 52, 600, 23, false);
	NEW_CLASS(this->btnSend, UI::GUIButton(ui, this, CSTR("Send")));
	this->btnSend->SetRect(104, 76, 75, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, CSTR("Dest File")));
	this->lblStatus->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR("")));
	this->txtStatus->SetRect(104, 100, 300, 23, false);
	this->txtStatus->SetReadOnly(true);
}

SSWR::AVIRead::AVIRFCMForm::~AVIRFCMForm()
{
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRFCMForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
