#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRACMEClientForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRACMEClientForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRACMEClientForm *me = (SSWR::AVIRead::AVIRACMEClientForm*)userObj;
	if (me->client)
	{
		DEL_CLASS(me->client);
		me->client = 0;
		me->txtHost->SetReadOnly(false);
		me->txtKeyFile->SetReadOnly(false);
		return;
	}
	UInt16 port = 0;
	UOSInt i;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbKey;
	me->txtHost->GetText(sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter host"), CSTR("ACME Client"), me);
		return;
	}
	i = sb.IndexOf(':');
	if (i != INVALID_INDEX)
	{
		if (!Text::StrToUInt16(sb.ToString() + i + 1, &port))
		{
			UI::MessageDialog::ShowDialog(CSTR("Port number in host name is not valid"), CSTR("ACME Client"), me);
			return;
		}
		sb.TrimToLength(i);
	}
	me->txtKeyFile->GetText(sbKey);
	if (sbKey.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter key file"), CSTR("ACME Client"), me);
		return;
	}
	NEW_CLASS(me->client, Net::ACMEClient(me->sockf, sb.ToCString(), port, sbKey.ToCString()));
	if (me->client->IsError())
	{
		UI::MessageDialog::ShowDialog(CSTR("Server does not have valid response"), CSTR("ACME Client"), me);
		DEL_CLASS(me->client);
		me->client = 0;
		return;
	}
	me->txtHost->SetReadOnly(true);
	me->txtKeyFile->SetReadOnly(true);
	Text::String *s = me->client->GetTermOfService();
	if (s)
	{
		me->txtTermOfService->SetText(s->ToCString());
	}
	else
	{
		me->txtTermOfService->SetText(CSTR(""));
	}
	s = me->client->GetWebsite();
	if (s)
	{
		me->txtWebsite->SetText(s->ToCString());
	}
	else
	{
		me->txtWebsite->SetText(CSTR(""));
	}
	s = me->client->GetAccountId();
	if (s)
	{
		me->txtAccount->SetText(s->ToCString());
	}
	else
	{
		me->txtAccount->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRACMEClientForm::AVIRACMEClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("ACME Client"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->client = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("ACMEKey.pem"));
	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this, CSTR("Host")));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this, CSTR("acme-staging-v02.api.letsencrypt.org")));
	this->txtHost->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->lblKeyFile, UI::GUILabel(ui, this, CSTR("KeyFile")));
	this->lblKeyFile->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtKeyFile, UI::GUITextBox(ui, this, CSTRP(sbuff, sptr)));
	this->txtKeyFile->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, CSTR("Start")));
	this->btnStart->SetRect(104, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);

	NEW_CLASS(this->lblTermOfService, UI::GUILabel(ui, this, CSTR("TermOfService")));
	this->lblTermOfService->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtTermOfService, UI::GUITextBox(ui, this, CSTR("")));
	this->txtTermOfService->SetRect(104, 76, 300, 23, false);
	this->txtTermOfService->SetReadOnly(true);
	NEW_CLASS(this->lblWebsite, UI::GUILabel(ui, this, CSTR("Website")));
	this->lblWebsite->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtWebsite, UI::GUITextBox(ui, this, CSTR("")));
	this->txtWebsite->SetRect(104, 100, 300, 23, false);
	this->txtWebsite->SetReadOnly(true);
	NEW_CLASS(this->lblAccount, UI::GUILabel(ui, this, CSTR("Account")));
	this->lblAccount->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtAccount, UI::GUITextBox(ui, this, CSTR("")));
	this->txtAccount->SetRect(104, 124, 300, 23, false);
	this->txtAccount->SetReadOnly(true);
}

SSWR::AVIRead::AVIRACMEClientForm::~AVIRACMEClientForm()
{
	SDEL_CLASS(this->client);
}

void SSWR::AVIRead::AVIRACMEClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
