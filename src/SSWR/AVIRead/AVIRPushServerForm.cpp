#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRPushServerForm.h"

void __stdcall SSWR::AVIRead::AVIRPushServerForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPushServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPushServerForm>();
	if (me->svr)
	{
		DEL_CLASS(me->svr);
		me->log.ClearHandlers();
		me->svr = 0;
		me->txtAPIKey->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		return;
	}
	UInt16 port;
	Text::StringBuilderUTF8 sbAPIKey;
	me->txtPort->GetText(sbAPIKey);
	if (!sbAPIKey.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid port number"), CSTR("PushServer"), me);
		return;
	}
	sbAPIKey.ClearStr();
	me->txtAPIKey->GetText(sbAPIKey);
	if (sbAPIKey.leng == 0)
	{
		me->ui->ShowMsgOK(CSTR("API Key is empty"), CSTR("PushServer"), me);
		return;
	}
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("log"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("PushSvr"));
	me->log.AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
	NEW_CLASS(me->svr, Net::PushServer(me->core->GetSocketFactory(), me->ssl, port, sbAPIKey.ToCString(), me->log));
	if (me->svr->IsError())
	{
		DEL_CLASS(me->svr);
		me->svr = 0;
		me->log.ClearHandlers();
		me->ui->ShowMsgOK(CSTR("Error in listening to port"), CSTR("PushServer"), me);
		return;
	}
	me->txtAPIKey->SetReadOnly(true);
	me->txtPort->SetReadOnly(true);
}

SSWR::AVIRead::AVIRPushServerForm::AVIRPushServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 156, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("PushServer"));
	this->SetNoResize(true);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("8000"));
	this->txtPort->SetRect(104, 4, 600, 23, false);
	this->lblAPIKey = ui->NewLabel(*this, CSTR("API Key"));
	this->lblAPIKey->SetRect(4, 28, 100, 23, false);
	this->txtAPIKey = ui->NewTextBox(*this, CSTR(""));
	this->txtAPIKey->SetRect(104, 28, 300, 23, false);
	this->btnStart = ui->NewButton(*this, CSTR("Start"));
	this->btnStart->SetRect(104, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
}

SSWR::AVIRead::AVIRPushServerForm::~AVIRPushServerForm()
{
	SDEL_CLASS(this->svr);
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRPushServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
