#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRACMEClientForm.h"

void __stdcall SSWR::AVIRead::AVIRACMEClientForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRACMEClientForm> me = userObj.GetNN<SSWR::AVIRead::AVIRACMEClientForm>();
	NN<Net::ACMEClient> client;
	if (me->client.SetTo(client))
	{
		client.Delete();
		me->client = nullptr;
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
		me->ui->ShowMsgOK(CSTR("Please enter host"), CSTR("ACME Client"), me);
		return;
	}
	i = sb.IndexOf(':');
	if (i != INVALID_INDEX)
	{
		if (!Text::StrToUInt16(sb.ToString() + i + 1, port))
		{
			me->ui->ShowMsgOK(CSTR("Port number in host name is not valid"), CSTR("ACME Client"), me);
			return;
		}
		sb.TrimToLength(i);
	}
	me->txtKeyFile->GetText(sbKey);
	if (sbKey.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter key file"), CSTR("ACME Client"), me);
		return;
	}
	NEW_CLASSNN(client, Net::ACMEClient(me->clif, sb.ToCString(), port, sbKey.ToCString()));
	if (client->IsError())
	{
		me->ui->ShowMsgOK(CSTR("Server does not have valid response"), CSTR("ACME Client"), me);
		client.Delete();
		return;
	}
	me->txtHost->SetReadOnly(true);
	me->txtKeyFile->SetReadOnly(true);
	me->client = client;
	NN<Text::String> s;
	if (client->GetTermOfService().SetTo(s))
	{
		me->txtTermOfService->SetText(s->ToCString());
	}
	else
	{
		me->txtTermOfService->SetText(CSTR(""));
	}
	if (client->GetWebsite().SetTo(s))
	{
		me->txtWebsite->SetText(s->ToCString());
	}
	else
	{
		me->txtWebsite->SetText(CSTR(""));
	}
	if (client->GetAccountId().SetTo(s))
	{
		me->txtAccount->SetText(s->ToCString());
	}
	else
	{
		me->txtAccount->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRACMEClientForm::AVIRACMEClientForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("ACME Client"));

	this->core = core;
	this->clif = core->GetTCPClientFactory();
	this->client = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("ACMEKey.pem"));
	this->lblHost = ui->NewLabel(*this, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(*this, CSTR("acme-staging-v02.api.letsencrypt.org"));
	this->txtHost->SetRect(104, 4, 200, 23, false);
	this->lblKeyFile = ui->NewLabel(*this, CSTR("KeyFile"));
	this->lblKeyFile->SetRect(4, 28, 100, 23, false);
	this->txtKeyFile = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtKeyFile->SetRect(104, 28, 200, 23, false);
	this->btnStart = ui->NewButton(*this, CSTR("Start"));
	this->btnStart->SetRect(104, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);

	this->lblTermOfService = ui->NewLabel(*this, CSTR("TermOfService"));
	this->lblTermOfService->SetRect(4, 76, 100, 23, false);
	this->txtTermOfService = ui->NewTextBox(*this, CSTR(""));
	this->txtTermOfService->SetRect(104, 76, 300, 23, false);
	this->txtTermOfService->SetReadOnly(true);
	this->lblWebsite = ui->NewLabel(*this, CSTR("Website"));
	this->lblWebsite->SetRect(4, 100, 100, 23, false);
	this->txtWebsite = ui->NewTextBox(*this, CSTR(""));
	this->txtWebsite->SetRect(104, 100, 300, 23, false);
	this->txtWebsite->SetReadOnly(true);
	this->lblAccount = ui->NewLabel(*this, CSTR("Account"));
	this->lblAccount->SetRect(4, 124, 100, 23, false);
	this->txtAccount = ui->NewTextBox(*this, CSTR(""));
	this->txtAccount->SetRect(104, 124, 300, 23, false);
	this->txtAccount->SetReadOnly(true);
}

SSWR::AVIRead::AVIRACMEClientForm::~AVIRACMEClientForm()
{
	this->client.Delete();
}

void SSWR::AVIRead::AVIRACMEClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
