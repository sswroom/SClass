#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRMySQLServerForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#define MYSQLVERSION CSTR("AVIRead-1.0")

void __stdcall SSWR::AVIRead::AVIRMySQLServerForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMySQLServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMySQLServerForm>();
	if (me->svr.NotNull())
	{
		me->svr.Delete();
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
	NN<DB::DBMS> dbms;
	NN<Net::MySQLServer> svr;
	NEW_CLASSNN(dbms, DB::DBMS(MYSQLVERSION, me->log));
	NEW_CLASSNN(svr, Net::MySQLServer(me->core->GetSocketFactory(), 0, port, dbms, true));
	if (svr->IsError())
	{
		svr.Delete();
	}
	else
	{
		me->svr = svr;
		dbms->UserAdd(1, CSTR("root"), CSTR("12345678"), CSTR("*"));
		me->txtPort->SetReadOnly(true);
	}
}

void __stdcall SSWR::AVIRead::AVIRMySQLServerForm::OnUserAddClicked(AnyType userObj)
{

}

void __stdcall SSWR::AVIRead::AVIRMySQLServerForm::OnLogSel(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMySQLServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMySQLServerForm>();
	Optional<Text::String> s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(Text::String::OrEmpty(s)->ToCString());
	OPTSTR_DEL(s);
}

void __stdcall SSWR::AVIRead::AVIRMySQLServerForm::OnTimerTick(AnyType userObj)
{
//	NN<SSWR::AVIRead::AVIRMySQLServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMySQLServerForm>();
}

SSWR::AVIRead::AVIRMySQLServerForm::AVIRMySQLServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("MySQL Server"));
	this->SetFont(nullptr, 8.25, false);
	this->svr = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->pnlCtrl, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlCtrl, CSTR("3306"));
	this->txtPort->SetRect(104, 4, 80, 23, false);
	this->btnStart = ui->NewButton(this->pnlCtrl, CSTR("Start"));
	this->btnStart->SetRect(184, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpUser = this->tcMain->AddTabPage(CSTR("User"));
	this->lbUser = ui->NewListBox(this->tpUser, false);
	this->lbUser->SetRect(0, 0, 100, 23, false);
	this->lbUser->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->hspUser = ui->NewHSplitter(this->tpUser, 3, false);
	this->pnlUser = ui->NewPanel(this->tpUser);
	this->pnlUser->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblUserName = ui->NewLabel(this->pnlUser, CSTR("User Name"));
	this->lblUserName->SetRect(8, 8, 100, 23, false);
	this->txtUserName = ui->NewTextBox(this->pnlUser, CSTR("root"));
	this->txtUserName->SetRect(108, 8, 200, 23, false);
	this->lblPassword = ui->NewLabel(this->pnlUser, CSTR("Password"));
	this->lblPassword->SetRect(8, 32, 100, 23, false);
	this->txtPassword = ui->NewTextBox(this->pnlUser, CSTR(""));
	this->txtPassword->SetRect(108, 32, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	this->btnUserAdd = ui->NewButton(this->pnlUser, CSTR("Add"));
	this->btnUserAdd->SetRect(100, 56, 75, 23, false);
	this->btnUserAdd->HandleButtonClick(OnUserAddClicked, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	this->AddTimer(1000, OnTimerTick, this);
	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 300, true));
	this->logger->SetTimeFormat("yyyy-MM-dd HH:mm:ss.fff");
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Command);
}

SSWR::AVIRead::AVIRMySQLServerForm::~AVIRMySQLServerForm()
{
	this->svr.Delete();
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
}

void SSWR::AVIRead::AVIRMySQLServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
