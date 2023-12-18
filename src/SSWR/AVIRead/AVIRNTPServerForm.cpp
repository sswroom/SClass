#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRNTPServerForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRNTPServerForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRNTPServerForm *me = (SSWR::AVIRead::AVIRNTPServerForm*)userObj;
	if (me->svr)
	{
		DEL_CLASS(me->svr);
		me->txtTimeServer->SetReadOnly(false);
		me->txtPort->SetReadOnly(false);
		return;
	}
	UInt16 port = 0;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(sb);
	if (!Text::StrToUInt16(sb.ToString(), port))
	{
		return;
	}
	if (port == 0)
	{
		return;
	}
	sb.ClearStr();
	me->txtTimeServer->GetText(sb);
	Net::SocketUtil::AddressInfo addr;
	NotNullPtr<Net::SocketFactory> sockf = me->core->GetSocketFactory();
	if (!sockf->DNSResolveIP(sb.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Cannot resolve the time server"), CSTR("Resolve"), me);
		return;
	}
	NEW_CLASS(me->svr, Net::NTPServer(me->core->GetSocketFactory(), port, me->log, sb.ToCString()));
	if (me->svr->IsError())
	{
		SDEL_CLASS(me->svr);
	}
	else
	{
		me->txtTimeServer->SetReadOnly(true);
		me->txtPort->SetReadOnly(true);
	}
}

void __stdcall SSWR::AVIRead::AVIRNTPServerForm::OnLogSel(void *userObj)
{
	SSWR::AVIRead::AVIRNTPServerForm *me = (SSWR::AVIRead::AVIRNTPServerForm*)userObj;
	Optional<Text::String> s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(Text::String::OrEmpty(s)->ToCString());
	OPTSTR_DEL(s);
}

void __stdcall SSWR::AVIRead::AVIRNTPServerForm::OnTimerTick(void *userObj)
{
//	SSWR::AVIRead::AVIRNTPServerForm *me = (SSWR::AVIRead::AVIRNTPServerForm*)userObj;
}

SSWR::AVIRead::AVIRNTPServerForm::AVIRNTPServerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("NTP Server"));
	this->SetFont(0, 0, 8.25, false);
	this->svr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->lblPort = ui->NewLabel(this->tpControl, CSTR("Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->tpControl, CSTR("123")));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	this->lblTimeServer = ui->NewLabel(this->tpControl, CSTR("Time Server"));
	this->lblTimeServer->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtTimeServer, UI::GUITextBox(ui, this->tpControl, CSTR("stdtime.gov.hk")));
	this->txtTimeServer->SetRect(108, 32, 500, 23, false);
	this->btnStart = ui->NewButton(this->tpControl, CSTR("Start"));
	this->btnStart->SetRect(100, 56, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	this->AddTimer(1000, OnTimerTick, this);
	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 300, true));
	this->logger->SetTimeFormat("yyyy-MM-dd HH:mm:ss.fff");
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Command);
}

SSWR::AVIRead::AVIRNTPServerForm::~AVIRNTPServerForm()
{
	SDEL_CLASS(this->svr);
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRNTPServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
