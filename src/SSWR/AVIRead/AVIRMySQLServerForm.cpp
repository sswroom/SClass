#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRMySQLServerForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#define MYSQLVERSION CSTR("AVIRead-1.0")

void __stdcall SSWR::AVIRead::AVIRMySQLServerForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMySQLServerForm *me = (SSWR::AVIRead::AVIRMySQLServerForm*)userObj;
	if (me->svr)
	{
		DEL_CLASS(me->svr);
		me->svr = 0;
		me->txtPort->SetReadOnly(false);
		return;
	}
	UInt16 port = 0;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(sb);
	if (!Text::StrToUInt16(sb.ToString(), &port))
	{
		return;
	}
	if (port == 0)
	{
		return;
	}
	sb.ClearStr();
	NEW_CLASS(me->dbms, DB::DBMS(MYSQLVERSION, me->log));
	NEW_CLASS(me->svr, Net::MySQLServer(me->core->GetSocketFactory(), port, me->dbms, true));
	if (me->svr->IsError())
	{
		SDEL_CLASS(me->svr);
	}
	else
	{
		me->dbms->UserAdd(1, CSTR("root"), CSTR("12345678"), CSTR("*"));
		me->txtPort->SetReadOnly(true);
	}
}

void __stdcall SSWR::AVIRead::AVIRMySQLServerForm::OnUserAddClicked(void *userObj)
{

}

void __stdcall SSWR::AVIRead::AVIRMySQLServerForm::OnLogSel(void *userObj)
{
	SSWR::AVIRead::AVIRMySQLServerForm *me = (SSWR::AVIRead::AVIRMySQLServerForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(s->ToCString());
	s->Release();
}

void __stdcall SSWR::AVIRead::AVIRMySQLServerForm::OnTimerTick(void *userObj)
{
//	SSWR::AVIRead::AVIRTFTPServerForm *me = (SSWR::AVIRead::AVIRTFTPServerForm*)userObj;
}

SSWR::AVIRead::AVIRMySQLServerForm::AVIRMySQLServerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("MySQL Server"));
	this->SetFont(0, 0, 8.25, false);
	this->svr = 0;
	this->log = 0;
	this->logger = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlCtrl, CSTR("Port")));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlCtrl, CSTR("3306")));
	this->txtPort->SetRect(104, 4, 80, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlCtrl, CSTR("Start")));
	this->btnStart->SetRect(184, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpUser = this->tcMain->AddTabPage(CSTR("User"));
	NEW_CLASS(this->lbUser, UI::GUIListBox(ui, this->tpUser, false));
	this->lbUser->SetRect(0, 0, 100, 23, false);
	this->lbUser->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspUser, UI::GUIHSplitter(ui, this->tpUser, 3, false));
	NEW_CLASS(this->pnlUser, UI::GUIPanel(ui, this->tpUser));
	this->pnlUser->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblUserName, UI::GUILabel(ui, this->pnlUser, CSTR("User Name")));
	this->lblUserName->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtUserName, UI::GUITextBox(ui, this->pnlUser, CSTR("root")));
	this->txtUserName->SetRect(108, 8, 200, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this->pnlUser, CSTR("Password")));
	this->lblPassword->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this->pnlUser, CSTR("")));
	this->txtPassword->SetRect(108, 32, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	NEW_CLASS(this->btnUserAdd, UI::GUIButton(ui, this->pnlUser, CSTR("Add")));
	this->btnUserAdd->SetRect(100, 56, 75, 23, false);
	this->btnUserAdd->HandleButtonClick(OnUserAddClicked, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);

	this->AddTimer(1000, OnTimerTick, this);
	NEW_CLASS(this->log, IO::LogTool());
	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 300, true));
	this->logger->SetTimeFormat("yyyy-MM-dd HH:mm:ss.fff");
	this->log->AddLogHandler(this->logger, IO::LogHandler::LogLevel::Command);
}

SSWR::AVIRead::AVIRMySQLServerForm::~AVIRMySQLServerForm()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIRMySQLServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
