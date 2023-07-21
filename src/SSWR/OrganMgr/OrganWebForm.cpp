#include "Stdafx.h"
#include "DB/MDBFileConn.h"
#include "DB/ODBCConn.h"
#include "IO/IniFile.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "SSWR/OrganMgr/OrganWebForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::OrganMgr::OrganWebForm::OnReloadClicked(void *userObj)
{
	SSWR::OrganMgr::OrganWebForm *me = (SSWR::OrganMgr::OrganWebForm*)userObj;
	me->env->Reload();
}

SSWR::OrganMgr::OrganWebForm::OrganWebForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<Media::DrawEngine> eng) : UI::GUIForm(parent, 160, 100, ui)
{
	this->SetFont(0, 0, 10.5, false);
	this->SetText(CSTR("OrganWeb"));
	this->SetNoResize(true);

	this->env = 0;
	NEW_CLASS(this->sockf, Net::OSSocketFactory(true));
	NEW_CLASS(this->log, IO::LogTool());

	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in loading config file"), CSTR("Error"), this);
		return;
	}

	DB::DBTool *db;
	Int32 scnSize = 0;
	Text::String *s;
	if ((s = cfg->GetValue(CSTR("ScreenSize"))) != 0)
	{
		scnSize = s->ToInt32();
	}
	if (scnSize <= 0)
	{
		scnSize = 1800;
	}
	if ((s = cfg->GetValue(CSTR("MDBFile"))) != 0)
	{
		db = DB::MDBFileConn::CreateDBTool(s, this->log, CSTR("DB: "));
	}
	else if ((s = cfg->GetValue(CSTR("MySQLServer"))) != 0)
	{
		db = Net::MySQLTCPClient::CreateDBTool(this->sockf, s, cfg->GetValue(CSTR("MySQLDB")), cfg->GetValue(CSTR("MySQLUID")), cfg->GetValue(CSTR("MySQLPwd")), this->log, CSTR("DB: "));
	}
	else
	{
		db = DB::ODBCConn::CreateDBTool(cfg->GetValue(CSTR("DBDSN")), cfg->GetValue(CSTR("DBUID")), cfg->GetValue(CSTR("DBPwd")), cfg->GetValue(CSTR("DBSchema")), this->log, CSTR("DB: "));
	}
	UInt16 port = 0;
	UInt16 sslPort = 0;
	s = cfg->GetValue(CSTR("SvrPort"));
	if (s)
	{
		s->ToUInt16(&port);
	}
	NEW_CLASS(this->env, OrganWebEnv(this->sockf, 0, this->log, db, cfg->GetValue(CSTR("ImageDir")), port, 0, cfg->GetValue(CSTR("CacheDir")), cfg->GetValue(CSTR("DataDir")), scnSize, cfg->GetValue(CSTR("ReloadPwd")), 0, eng, cfg->GetValue(CSTR("OSMCacheOath"))->ToCString()));
	DEL_CLASS(cfg);

	NEW_CLASS(this->btnReload, UI::GUIButton(ui, this, CSTR("&Reload")));
	this->btnReload->SetRect(40, 16, 75, 23, false);
	this->btnReload->HandleButtonClick(OnReloadClicked, this);
	if (this->env->IsError())
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in starting server"), CSTR("Error"), this);
	}
}

SSWR::OrganMgr::OrganWebForm::~OrganWebForm()
{
	SDEL_CLASS(this->env);
	DEL_CLASS(this->sockf);
	DEL_CLASS(this->log);
}

Bool SSWR::OrganMgr::OrganWebForm::IsError()
{
	if (this->env == 0)
		return true;
	if (this->env->IsError())
		return true;
	return false;
}