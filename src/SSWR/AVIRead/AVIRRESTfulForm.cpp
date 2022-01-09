#include "Stdafx.h"
#include "DB/DBConn.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRMySQLConnForm.h"
#include "SSWR/AVIRead/AVIRODBCDSNForm.h"
#include "SSWR/AVIRead/AVIRRESTfulForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRRESTfulForm::OnDatabaseMySQLClicked(void *userObj)
{
	SSWR::AVIRead::AVIRRESTfulForm *me = (SSWR::AVIRead::AVIRRESTfulForm*)userObj;
	SSWR::AVIRead::AVIRMySQLConnForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRMySQLConnForm(0, me->GetUI(), me->core));
	if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->dbCache);
		SDEL_CLASS(me->dbModel);
		SDEL_CLASS(me->db);
		SDEL_CLASS(me->dbConn);
		me->dbConn = frm->GetDBConn();
		me->InitDB();
	}
	DEL_CLASS(frm);
}

void __stdcall SSWR::AVIRead::AVIRRESTfulForm::OnDatabaseODBCDSNClicked(void *userObj)
{
	SSWR::AVIRead::AVIRRESTfulForm *me = (SSWR::AVIRead::AVIRRESTfulForm*)userObj;
	SSWR::AVIRead::AVIRODBCDSNForm *frm;
	NEW_CLASS(frm, SSWR::AVIRead::AVIRODBCDSNForm(0, me->GetUI(), me->core));
	if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->dbCache);
		SDEL_CLASS(me->dbModel);
		SDEL_CLASS(me->db);
		SDEL_CLASS(me->dbConn);
		me->dbConn = frm->GetDBConn();
		me->InitDB();
	}
	DEL_CLASS(frm);
}

void __stdcall SSWR::AVIRead::AVIRRESTfulForm::OnStartClick(void *userObj)
{
	SSWR::AVIRead::AVIRRESTfulForm *me = (SSWR::AVIRead::AVIRRESTfulForm*)userObj;
	if (me->svr)
	{
		return;
	}
	if (me->dbConn == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"No database is connected", (const UTF8Char*)"RESTful Server", me);
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	Text::StringBuilderUTF8 *sb;
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	me->txtPort->GetText(sb);
	if (sb->ToUInt16(&port) && port > 0 && port <= 65535)
	{
		NEW_CLASS(me->restHdlr, Net::WebServer::RESTfulHandler(me->dbCache));
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), 0, me->restHdlr, port, 120, Sync::Thread::GetThreadCnt(), (const UTF8Char*)"sswr", me->chkAllowProxy->IsChecked(), me->chkAllowKA->IsChecked()));
		if (me->svr->IsError())
		{
			valid = false;
			SDEL_CLASS(me->svr);
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in starting server", (const UTF8Char*)"RESTful Server", me);
		}
		else
		{
			sb->ClearStr();
			me->txtLogDir->GetText(sb);
			if (sb->GetEndPtr()[-1] != IO::Path::PATH_SEPERATOR)
			{
				sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
			}
			sb->AppendC(UTF8STRC("Acccess"));

			if (!me->chkSkipLog->IsChecked())
			{
				NEW_CLASS(me->log, IO::LogTool());
				me->log->AddFileLog(sb->ToString(), IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
				me->svr->SetAccessLog(me->log, IO::ILogHandler::LOG_LEVEL_RAW);
				NEW_CLASS(me->logger, UI::ListBoxLogger(me, me->lbLog, 500, true));
				me->log->AddLogHandler(me->logger, IO::ILogHandler::LOG_LEVEL_RAW);
			}

			me->lvTable->ClearItems();
			Data::ArrayList<const UTF8Char*> tableNames;
			const UTF8Char *tableName;
			UTF8Char sbuff[32];
			UOSInt i;
			UOSInt j;
			UOSInt k;
			me->dbModel->GetTableNames(&tableNames);
			i = 0;
			j = tableNames.GetCount();
			while (i < j)
			{
				tableName = tableNames.GetItem(i);
				k = me->lvTable->AddItem(tableName, 0);
				Text::StrOSInt(sbuff, me->dbCache->GetRowCount(tableName));
				me->lvTable->SetSubItem(k, 1, sbuff);
				i++;
			}

		}

		if (valid)
		{
			me->txtPort->SetReadOnly(true);
			me->txtLogDir->SetReadOnly(true);
			me->chkAllowProxy->SetEnabled(false);
			me->chkSkipLog->SetEnabled(false);
			me->chkAllowKA->SetEnabled(false);
			me->btnDatabaseMySQL->SetEnabled(false);
			me->btnDatabaseODBCDSN->SetEnabled(false);
		}
		else
		{
			SDEL_CLASS(me->svr);
			if (me->restHdlr)
			{
				me->restHdlr->Release();
				me->restHdlr = 0;
			}
			SDEL_CLASS(me->log);
			SDEL_CLASS(me->logger);
		}
	}
	else
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Port is not valid", (const UTF8Char*)"RESTful Server", me);
	}

	DEL_CLASS(sb);
}

void __stdcall SSWR::AVIRead::AVIRRESTfulForm::OnStopClick(void *userObj)
{
	SSWR::AVIRead::AVIRRESTfulForm *me = (SSWR::AVIRead::AVIRRESTfulForm*)userObj;
	if (me->svr == 0)
	{
		return;
	}
	SDEL_CLASS(me->svr);
	if (me->restHdlr)
	{
		me->restHdlr->Release();
		me->restHdlr = 0;
	}
	SDEL_CLASS(me->log);
	SDEL_CLASS(me->logger);
	me->txtPort->SetReadOnly(false);
	me->txtLogDir->SetReadOnly(false);
	me->chkAllowProxy->SetEnabled(true);
	me->chkSkipLog->SetEnabled(true);
	me->chkAllowKA->SetEnabled(true);
	me->btnDatabaseMySQL->SetEnabled(true);
	me->btnDatabaseODBCDSN->SetEnabled(true);
}

void __stdcall SSWR::AVIRead::AVIRRESTfulForm::OnLogSel(void *userObj)
{
	SSWR::AVIRead::AVIRRESTfulForm *me = (SSWR::AVIRead::AVIRRESTfulForm*)userObj;
	Text::String *s = me->lbLog->GetSelectedItemTextNew();
	me->txtLog->SetText(s->v);
	s->Release();
}

void SSWR::AVIRead::AVIRRESTfulForm::InitDB()
{
	Text::StringBuilderUTF8 sb;
	this->dbConn->GetConnName(&sb);
	NEW_CLASS(this->db, DB::DBTool(this->dbConn, false, this->log, (const UTF8Char*)"DB: "));
	NEW_CLASS(this->dbModel, DB::DBModel());
	this->dbModel->LoadDatabase(this->db, 0);
	NEW_CLASS(this->dbCache, DB::DBCache(this->dbModel, this->db));
	this->txtDatabase->SetText(sb.ToString());
}

SSWR::AVIRead::AVIRRESTfulForm::AVIRRESTfulForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UOSInt i;
	this->core = core;
	this->SetText((const UTF8Char*)"RESTful Server");
	this->SetFont(0, 0, 8.25, false);
	this->svr = 0;
	this->log = 0;
	this->logger = 0;
	this->db = 0;
	this->dbConn = 0;
	this->dbCache = 0;
	this->dbModel = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage((const UTF8Char*)"Control");
	NEW_CLASS(this->grpParam, UI::GUIGroupBox(ui, this->tpControl, (const UTF8Char*)"Parameters"));
	this->grpParam->SetRect(0, 0, 620, 176, false);
	this->grpParam->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->grpParam, (const UTF8Char*)"12345"));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	NEW_CLASS(this->lblLogDir, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Log Path"));
	this->lblLogDir->SetRect(8, 32, 100, 23, false);
	IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sbuff[i] = IO::Path::PATH_SEPERATOR;
	Text::StrConcatC(&sbuff[i+1], UTF8STRC("log"));
	NEW_CLASS(this->txtLogDir, UI::GUITextBox(ui, this->grpParam, sbuff));
	this->txtLogDir->SetRect(108, 32, 500, 23, false);
	NEW_CLASS(this->lblAllowProxy, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Proxy Connection"));
	this->lblAllowProxy->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->chkAllowProxy, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Allow", true));
	this->chkAllowProxy->SetRect(108, 56, 100, 23, false);
	NEW_CLASS(this->lblSkipLog, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Skip Logging"));
	this->lblSkipLog->SetRect(8, 80, 100, 23, false);
	NEW_CLASS(this->chkSkipLog, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Enable", false));
	this->chkSkipLog->SetRect(108, 80, 100, 23, false);
	NEW_CLASS(this->lblAllowKA, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Allow KA"));
	this->lblAllowKA->SetRect(8, 104, 100, 23, false);
	NEW_CLASS(this->chkAllowKA, UI::GUICheckBox(ui, this->grpParam, (const UTF8Char*)"Enable", true));
	this->chkAllowKA->SetRect(108, 104, 100, 23, false);
	NEW_CLASS(this->lblDatabase, UI::GUILabel(ui, this->grpParam, (const UTF8Char*)"Database"));
	this->lblDatabase->SetRect(8, 128, 100, 23, false);
	NEW_CLASS(this->txtDatabase, UI::GUITextBox(ui, this->grpParam, (const UTF8Char*)""));
	this->txtDatabase->SetRect(108, 128, 150, 23, false);
	this->txtDatabase->SetReadOnly(true);
	NEW_CLASS(this->btnDatabaseMySQL, UI::GUIButton(ui, this->grpParam, (const UTF8Char*)"MySQL"));
	this->btnDatabaseMySQL->SetRect(258, 128, 75, 23, false);
	this->btnDatabaseMySQL->HandleButtonClick(OnDatabaseMySQLClicked, this);
	NEW_CLASS(this->btnDatabaseODBCDSN, UI::GUIButton(ui, this->grpParam, (const UTF8Char*)"ODBC"));
	this->btnDatabaseODBCDSN->SetRect(338, 128, 75, 23, false);
	this->btnDatabaseODBCDSN->HandleButtonClick(OnDatabaseODBCDSNClicked, this);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(200, 188, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this->tpControl, (const UTF8Char*)"Stop"));
	this->btnStop->SetRect(300, 188, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClick, this);

	this->tpTable = this->tcMain->AddTabPage((const UTF8Char*)"Table");
	NEW_CLASS(this->lvTable, UI::GUIListView(ui, this->tpTable, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTable->SetFullRowSelect(true);
	this->lvTable->SetShowGrid(true);
	this->lvTable->AddColumn((const UTF8Char*)"TableName", 150);
	this->lvTable->AddColumn((const UTF8Char*)"Row Count", 100);

	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, (const UTF8Char*)""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);
}

SSWR::AVIRead::AVIRRESTfulForm::~AVIRRESTfulForm()
{
	SDEL_CLASS(this->svr);
	if (this->restHdlr)
	{
		this->restHdlr->Release();
		this->restHdlr = 0;
	}
	SDEL_CLASS(this->dbCache);
	SDEL_CLASS(this->dbModel);
	SDEL_CLASS(this->db);
	SDEL_CLASS(this->dbConn);
	SDEL_CLASS(this->log);
	SDEL_CLASS(this->logger);
}

void SSWR::AVIRead::AVIRRESTfulForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
