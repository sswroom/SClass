#include "Stdafx.h"
#include "DB/MDBFile.h"
#include "DB/ODBCConn.h"
#include "IO/IniFile.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "SSWR/OrganMgr/OrganWebForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::OrganMgr::OrganWebForm::OnReloadClicked(void *userObj)
{
	SSWR::OrganMgr::OrganWebForm *me = (SSWR::OrganMgr::OrganWebForm*)userObj;
	me->dataHdlr->Reload();
}

SSWR::OrganMgr::OrganWebForm::OrganWebForm(UI::GUIClientControl *parent, UI::GUICore *ui, Media::DrawEngine *eng) : UI::GUIForm(parent, 160, 100, ui)
{
	this->SetFont(0, 10.5, false);
	this->SetText((const UTF8Char*)"OrganWeb");
	this->SetNoResize(true);

	this->dataHdlr = 0;
	NEW_CLASS(this->sockf, Net::OSSocketFactory(true));
	NEW_CLASS(this->log, IO::LogTool());

	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in loading config file", (const UTF8Char*)"Error", this);
		return;
	}

	DB::DBTool *db;
	Int32 scnSize = 0;
	if (cfg->GetValue((const UTF8Char*)"ScreenSize"))
	{
		scnSize = Text::StrToInt32(cfg->GetValue((const UTF8Char*)"ScreenSize"));
	}
	if (scnSize <= 0)
	{
		scnSize = 1800;
	}
	if (cfg->GetValue((const UTF8Char*)"MDBFile"))
	{
		db = DB::MDBFile::CreateDBTool(cfg->GetValue((const UTF8Char*)"MDBFile"), this->log, (const UTF8Char*)"DB: ");
	}
	else if (cfg->GetValue((const UTF8Char*)"MySQLServer"))
	{
		db = Net::MySQLTCPClient::CreateDBTool(this->sockf, cfg->GetValue((const UTF8Char*)"MySQLServer"), cfg->GetValue((const UTF8Char*)"MySQLDB"), cfg->GetValue((const UTF8Char*)"MySQLUID"), cfg->GetValue((const UTF8Char*)"MySQLPwd"), this->log, (const UTF8Char*)"DB: ");
	}
	else
	{
		db = DB::ODBCConn::CreateDBTool(cfg->GetValue((const UTF8Char*)"DBDSN"), cfg->GetValue((const UTF8Char*)"DBUID"), cfg->GetValue((const UTF8Char*)"DBPwd"), cfg->GetValue((const UTF8Char*)"DBSchema"), this->log, (const UTF8Char*)"DB: ");
	}
	UInt16 port = 0;
	Text::StrToUInt16(cfg->GetValue((const UTF8Char*)"SvrPort"), &port);
	NEW_CLASS(this->dataHdlr, OrganWebHandler(this->sockf, this->log, db, cfg->GetValue((const UTF8Char*)"ImageDir"), port, cfg->GetValue((const UTF8Char*)"CacheDir"), cfg->GetValue((const UTF8Char*)"DataDir"), scnSize, cfg->GetValue((const UTF8Char*)"ReloadPwd"), 0, eng));
	DEL_CLASS(cfg);

	NEW_CLASS(this->btnReload, UI::GUIButton(ui, this, (const UTF8Char*)"&Reload"));
	this->btnReload->SetRect(40, 16, 75, 23, false);
	this->btnReload->HandleButtonClick(OnReloadClicked, this);
	if (this->dataHdlr->IsError())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in starting server", (const UTF8Char*)"Error", this);
	}
}

SSWR::OrganMgr::OrganWebForm::~OrganWebForm()
{
	SDEL_CLASS(this->dataHdlr);
	DEL_CLASS(this->sockf);
	DEL_CLASS(this->log);
}

Bool SSWR::OrganMgr::OrganWebForm::IsError()
{
	if (this->dataHdlr == 0)
		return true;
	if (this->dataHdlr->IsError())
		return true;
	return false;
}