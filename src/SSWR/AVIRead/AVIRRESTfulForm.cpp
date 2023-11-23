#include "Stdafx.h"
#include "DB/DBConn.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRMySQLConnForm.h"
#include "SSWR/AVIRead/AVIRODBCDSNForm.h"
#include "SSWR/AVIRead/AVIRRESTfulForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRRESTfulForm::OnDatabaseMySQLClicked(void *userObj)
{
	SSWR::AVIRead::AVIRRESTfulForm *me = (SSWR::AVIRead::AVIRRESTfulForm*)userObj;
	SSWR::AVIRead::AVIRMySQLConnForm frm(0, me->GetUI(), me->core);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->dbCache);
		SDEL_CLASS(me->dbModel);
		SDEL_CLASS(me->db);
		SDEL_CLASS(me->dbConn);
		me->dbConn = frm.GetDBConn();
		me->InitDB();
	}
}

void __stdcall SSWR::AVIRead::AVIRRESTfulForm::OnDatabaseODBCDSNClicked(void *userObj)
{
	SSWR::AVIRead::AVIRRESTfulForm *me = (SSWR::AVIRead::AVIRRESTfulForm*)userObj;
	SSWR::AVIRead::AVIRODBCDSNForm frm(0, me->GetUI(), me->core);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		SDEL_CLASS(me->dbCache);
		SDEL_CLASS(me->dbModel);
		SDEL_CLASS(me->db);
		SDEL_CLASS(me->dbConn);
		me->dbConn = frm.GetDBConn();
		me->InitDB();
	}
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
		UI::MessageDialog::ShowDialog(CSTR("No database is connected"), CSTR("RESTful Server"), me);
		return;
	}
	UInt16 port = 0;
	Bool valid = true;
	Text::StringBuilderUTF8 sb;
	me->txtPort->GetText(sb);
	if (sb.ToUInt16(port) && port > 0 && port <= 65535)
	{
		NotNullPtr<Net::WebServer::RESTfulHandler> restHdlr;
		NEW_CLASSNN(restHdlr, Net::WebServer::RESTfulHandler(me->dbCache));
		NEW_CLASS(me->svr, Net::WebServer::WebListener(me->core->GetSocketFactory(), 0, restHdlr, port, 120, 2, Sync::ThreadUtil::GetThreadCnt(), CSTR("sswr"), me->chkAllowProxy->IsChecked(), me->chkAllowKA->IsChecked()?Net::WebServer::KeepAlive::Always:Net::WebServer::KeepAlive::Default, false));
		if (me->svr->IsError())
		{
			valid = false;
			SDEL_CLASS(me->svr);
			restHdlr.Delete();
			UI::MessageDialog::ShowDialog(CSTR("Error in listening to port"), CSTR("RESTful Server"), me);
		}
		else
		{
			me->restHdlr = restHdlr.Ptr();
			sb.ClearStr();
			me->txtLogDir->GetText(sb);
			if (sb.GetEndPtr()[-1] != IO::Path::PATH_SEPERATOR)
			{
				sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			}
			sb.AppendC(UTF8STRC("Acccess"));

			if (!me->chkSkipLog->IsChecked())
			{
				me->log.AddFileLog(sb.ToCString(), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
				me->svr->SetAccessLog(&me->log, IO::LogHandler::LogLevel::Raw);
				NotNullPtr<UI::ListBoxLogger> logger;
				NEW_CLASSNN(logger, UI::ListBoxLogger(*me, me->lbLog, 500, true));
				me->logger = logger.Ptr();
				me->log.AddLogHandler(logger, IO::LogHandler::LogLevel::Raw);
			}

			me->lvTable->ClearItems();
			Data::ArrayList<Text::CString> tableNames;
			Text::CStringNN tableName;
			UTF8Char sbuff[32];
			UTF8Char *sptr;
			UOSInt i;
			UOSInt j;
			UOSInt k;
			me->dbModel->GetTableNames(&tableNames);
			i = 0;
			j = tableNames.GetCount();
			while (i < j)
			{
				tableName = tableNames.GetItem(i).OrEmpty();
				k = me->lvTable->AddItem(tableName, 0);
				sptr = Text::StrOSInt(sbuff, me->dbCache->GetRowCount(tableName));
				me->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				i++;
			}
			
			if (!me->svr->Start())
			{
				valid = false;
				UI::MessageDialog::ShowDialog(CSTR("Error in starting server"), CSTR("RESTful Server"), me);
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
			SDEL_CLASS(me->restHdlr);
			NotNullPtr<UI::ListBoxLogger> logger;
			if (logger.Set(me->logger))
			{
				me->log.RemoveLogHandler(logger);
				SDEL_CLASS(me->logger);
			}
		}
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Port is not valid"), CSTR("RESTful Server"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRRESTfulForm::OnStopClick(void *userObj)
{
	SSWR::AVIRead::AVIRRESTfulForm *me = (SSWR::AVIRead::AVIRRESTfulForm*)userObj;
	if (me->svr == 0)
	{
		return;
	}
	SDEL_CLASS(me->svr);
	SDEL_CLASS(me->restHdlr);
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
	me->txtLog->SetText(s->ToCString());
	s->Release();
}

void SSWR::AVIRead::AVIRRESTfulForm::InitDB()
{
	NotNullPtr<DB::DBTool> db;
	NotNullPtr<DB::DBModel> dbModel;
	NotNullPtr<DB::DBConn> dbConn;
	if (dbConn.Set(this->dbConn))
	{
		Text::StringBuilderUTF8 sb;
		dbConn->GetConnName(sb);
		NEW_CLASSNN(db, DB::DBTool(dbConn, false, this->log, CSTR("DB: ")));
		NEW_CLASSNN(dbModel, DB::DBModel());
		this->db = db.Ptr();
		this->dbModel = dbModel.Ptr();
		this->dbModel->LoadDatabase(this->db, CSTR_NULL, CSTR_NULL);
		NEW_CLASS(this->dbCache, DB::DBCache(dbModel, db));
		this->txtDatabase->SetText(sb.ToCString());
	}
}

SSWR::AVIRead::AVIRRESTfulForm::AVIRRESTfulForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	this->core = core;
	this->SetText(CSTR("RESTful Server"));
	this->SetFont(0, 0, 8.25, false);
	this->svr = 0;
	this->logger = 0;
	this->db = 0;
	this->dbConn = 0;
	this->dbCache = 0;
	this->dbModel = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	NEW_CLASSNN(this->grpParam, UI::GUIGroupBox(ui, this->tpControl, CSTR("Parameters")));
	this->grpParam->SetRect(0, 0, 620, 176, false);
	this->grpParam->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->grpParam, CSTR("Port")));
	this->lblPort->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->grpParam, CSTR("12345")));
	this->txtPort->SetRect(108, 8, 50, 23, false);
	NEW_CLASS(this->lblLogDir, UI::GUILabel(ui, this->grpParam, CSTR("Log Path")));
	this->lblLogDir->SetRect(8, 32, 100, 23, false);
	sptr = IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sbuff[i] = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(&sbuff[i+1], UTF8STRC("log"));
	NEW_CLASS(this->txtLogDir, UI::GUITextBox(ui, this->grpParam, CSTRP(sbuff, sptr)));
	this->txtLogDir->SetRect(108, 32, 500, 23, false);
	NEW_CLASS(this->lblAllowProxy, UI::GUILabel(ui, this->grpParam, CSTR("Proxy Connection")));
	this->lblAllowProxy->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->chkAllowProxy, UI::GUICheckBox(ui, this->grpParam, CSTR("Allow"), true));
	this->chkAllowProxy->SetRect(108, 56, 100, 23, false);
	NEW_CLASS(this->lblSkipLog, UI::GUILabel(ui, this->grpParam, CSTR("Skip Logging")));
	this->lblSkipLog->SetRect(8, 80, 100, 23, false);
	NEW_CLASS(this->chkSkipLog, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), false));
	this->chkSkipLog->SetRect(108, 80, 100, 23, false);
	NEW_CLASS(this->lblAllowKA, UI::GUILabel(ui, this->grpParam, CSTR("Allow KA")));
	this->lblAllowKA->SetRect(8, 104, 100, 23, false);
	NEW_CLASS(this->chkAllowKA, UI::GUICheckBox(ui, this->grpParam, CSTR("Enable"), true));
	this->chkAllowKA->SetRect(108, 104, 100, 23, false);
	NEW_CLASS(this->lblDatabase, UI::GUILabel(ui, this->grpParam, CSTR("Database")));
	this->lblDatabase->SetRect(8, 128, 100, 23, false);
	NEW_CLASS(this->txtDatabase, UI::GUITextBox(ui, this->grpParam, CSTR("")));
	this->txtDatabase->SetRect(108, 128, 150, 23, false);
	this->txtDatabase->SetReadOnly(true);
	NEW_CLASS(this->btnDatabaseMySQL, UI::GUIButton(ui, this->grpParam, CSTR("MySQL")));
	this->btnDatabaseMySQL->SetRect(258, 128, 75, 23, false);
	this->btnDatabaseMySQL->HandleButtonClick(OnDatabaseMySQLClicked, this);
	NEW_CLASS(this->btnDatabaseODBCDSN, UI::GUIButton(ui, this->grpParam, CSTR("ODBC")));
	this->btnDatabaseODBCDSN->SetRect(338, 128, 75, 23, false);
	this->btnDatabaseODBCDSN->HandleButtonClick(OnDatabaseODBCDSNClicked, this);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->tpControl, CSTR("Start")));
	this->btnStart->SetRect(200, 188, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClick, this);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this->tpControl, CSTR("Stop")));
	this->btnStop->SetRect(300, 188, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClick, this);

	this->tpTable = this->tcMain->AddTabPage(CSTR("Table"));
	NEW_CLASS(this->lvTable, UI::GUIListView(ui, this->tpTable, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTable->SetFullRowSelect(true);
	this->lvTable->SetShowGrid(true);
	this->lvTable->AddColumn(CSTR("TableName"), 150);
	this->lvTable->AddColumn(CSTR("Row Count"), 100);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSel, this);
}

SSWR::AVIRead::AVIRRESTfulForm::~AVIRRESTfulForm()
{
	SDEL_CLASS(this->svr);
	SDEL_CLASS(this->restHdlr);
	SDEL_CLASS(this->dbCache);
	SDEL_CLASS(this->dbModel);
	SDEL_CLASS(this->db);
	SDEL_CLASS(this->dbConn);
	NotNullPtr<UI::ListBoxLogger> logger;
	if (logger.Set(this->logger))
	{
		this->log.RemoveLogHandler(logger);
		SDEL_CLASS(this->logger);
	}
}

void SSWR::AVIRead::AVIRRESTfulForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
