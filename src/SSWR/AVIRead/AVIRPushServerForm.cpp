#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRPushServerForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRPushServerForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRPushServerForm *me = (SSWR::AVIRead::AVIRPushServerForm*)userObj;
	if (me->svr)
	{
		DEL_CLASS(me->svr);
		DEL_CLASS(me->log);
		me->svr = 0;
		me->log = 0;
		me->txtAPIKey->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		return;
	}
	UInt16 port;
	Text::StringBuilderUTF8 sbAPIKey;
	me->txtPort->GetText(sbAPIKey);
	if (!sbAPIKey.ToUInt16(port))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please enter valid port number"), CSTR("PushServer"), me);
		return;
	}
	sbAPIKey.ClearStr();
	me->txtAPIKey->GetText(sbAPIKey);
	if (sbAPIKey.leng == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("API Key is empty"), CSTR("PushServer"), me);
		return;
	}
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NEW_CLASS(me->log, IO::LogTool());
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("log"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("PushSvr"));
	me->log->AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
	NEW_CLASS(me->svr, Net::PushServer(me->core->GetSocketFactory(), me->ssl, port, sbAPIKey.ToCString(), me->log));
	if (me->svr->IsError())
	{
		DEL_CLASS(me->svr);
		DEL_CLASS(me->log);
		me->svr = 0;
		me->log = 0;
		UI::MessageDialog::ShowDialog(CSTR("Error in listening to port"), CSTR("PushServer"), me);
		return;
	}
	me->txtAPIKey->SetReadOnly(true);
	me->txtPort->SetReadOnly(true);
}

SSWR::AVIRead::AVIRPushServerForm::AVIRPushServerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 156, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("PushServer"));
	this->SetNoResize(true);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, CSTR("Port")));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, CSTR("8000")));
	this->txtPort->SetRect(104, 4, 600, 23, false);
	NEW_CLASS(this->lblAPIKey, UI::GUILabel(ui, this, CSTR("API Key")));
	this->lblAPIKey->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtAPIKey, UI::GUITextBox(ui, this, CSTR("")));
	this->txtAPIKey->SetRect(104, 28, 300, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, CSTR("Start")));
	this->btnStart->SetRect(104, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
}

SSWR::AVIRead::AVIRPushServerForm::~AVIRPushServerForm()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRPushServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
