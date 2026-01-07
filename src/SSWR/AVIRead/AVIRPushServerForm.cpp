#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRPushServerForm.h"

void __stdcall SSWR::AVIRead::AVIRPushServerForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPushServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPushServerForm>();
	if (me->svr.NotNull())
	{
		me->svr.Delete();
		me->serviceAccount.Delete();
		me->log.ClearHandlers();
		me->svr = 0;
		me->txtServiceAccount->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		return;
	}
	UInt16 port;
	Text::StringBuilderUTF8 sbServiceAccount;
	me->txtPort->GetText(sbServiceAccount);
	if (!sbServiceAccount.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("PushServer"), me);
		return;
	}
	sbServiceAccount.ClearStr();
	me->txtServiceAccount->GetText(sbServiceAccount);
	if (sbServiceAccount.leng == 0)
	{
		me->ui->ShowMsgOK(CSTR("Service Account is empty"), CSTR("PushServer"), me);
		return;
	}
	NN<Net::Google::GoogleServiceAccount> serviceAccount;
	if (!Net::Google::GoogleServiceAccount::FromFile(sbServiceAccount.ToCString()).SetTo(serviceAccount))
	{
		me->ui->ShowMsgOK(CSTR("Error in reading service account file"), CSTR("PushServer"), me);
		return;
	}
	me->serviceAccount = serviceAccount;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("log"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("PushSvr"));
	me->log.AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
	NN<Net::PushServer> svr;
	NEW_CLASSNN(svr, Net::PushServer(me->core->GetTCPClientFactory(), me->ssl, port, serviceAccount, me->log));
	if (svr->IsError())
	{
		svr.Delete();
		me->svr = 0;
		me->serviceAccount.Delete();
		me->log.ClearHandlers();
		me->ui->ShowMsgOK(CSTR("Error in listening to port"), CSTR("PushServer"), me);
		return;
	}
	me->svr = svr;
	me->txtServiceAccount->SetReadOnly(true);
	me->txtPort->SetReadOnly(true);
}

SSWR::AVIRead::AVIRPushServerForm::AVIRPushServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 156, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("PushServer"));
	this->SetNoResize(true);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->serviceAccount = 0;
	this->svr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("8000"));
	this->txtPort->SetRect(104, 4, 600, 23, false);
	this->lblServiceAccount = ui->NewLabel(*this, CSTR("Service Account"));
	this->lblServiceAccount->SetRect(4, 28, 100, 23, false);
	this->txtServiceAccount = ui->NewTextBox(*this, CSTR(""));
	this->txtServiceAccount->SetRect(104, 28, 300, 23, false);
	this->btnStart = ui->NewButton(*this, CSTR("Start"));
	this->btnStart->SetRect(104, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
}

SSWR::AVIRead::AVIRPushServerForm::~AVIRPushServerForm()
{
	this->svr.Delete();
	this->ssl.Delete();
	this->serviceAccount.Delete();
}

void SSWR::AVIRead::AVIRPushServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
