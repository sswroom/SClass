#include "Stdafx.h"
#include "DB/MDBFileConn.h"
#include "DB/ODBCConn.h"
#include "IO/IniFile.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "SSWR/OrganMgr/OrganWebForm.h"

void __stdcall SSWR::OrganMgr::OrganWebForm::OnReloadClicked(AnyType userObj)
{
	NN<SSWR::OrganMgr::OrganWebForm> me = userObj.GetNN<SSWR::OrganMgr::OrganWebForm>();
	me->env->Reload();
}

SSWR::OrganMgr::OrganWebForm::OrganWebForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::DrawEngine> eng) : UI::GUIForm(parent, 160, 100, ui)
{
	this->SetFont(0, 0, 10.5, false);
	this->SetText(CSTR("OrganWeb"));
	this->SetNoResize(true);

	this->env = 0;
	NEW_CLASSNN(this->sockf, Net::OSSocketFactory(true));
	NEW_CLASSNN(this->clif, Net::TCPClientFactory(this->sockf));

	NN<IO::ConfigFile> cfg;
	if (!IO::IniFile::ParseProgConfig(0).SetTo(cfg))
	{
		ui->ShowMsgOK(CSTR("Error in loading config file"), CSTR("Error"), this);
		return;
	}

	Optional<DB::DBTool> db;
	Int32 scnSize = 0;
	NN<Text::String> s;
	if (cfg->GetValue(CSTR("ScreenSize")).SetTo(s))
	{
		scnSize = s->ToInt32();
	}
	if (scnSize <= 0)
	{
		scnSize = 1800;
	}
	if (cfg->GetValue(CSTR("MDBFile")).SetTo(s))
	{
		db = DB::MDBFileConn::CreateDBTool(s, this->log, CSTR("DB: "));
	}
	else if (cfg->GetValue(CSTR("MySQLServer")).SetTo(s))
	{
		db = Net::MySQLTCPClient::CreateDBTool(this->clif, s, cfg->GetValue(CSTR("MySQLDB")), Text::String::OrEmpty(cfg->GetValue(CSTR("MySQLUID"))), Text::String::OrEmpty(cfg->GetValue(CSTR("MySQLPwd"))), this->log, CSTR("DB: "));
	}
	else
	{
		db = DB::ODBCConn::CreateDBTool(Text::String::OrEmpty(cfg->GetValue(CSTR("DBDSN"))), cfg->GetValue(CSTR("DBUID")), cfg->GetValue(CSTR("DBPwd")), cfg->GetValue(CSTR("DBSchema")), this->log, CSTR("DB: "));
	}
	UInt16 port = 0;
	UInt16 sslPort = 0;
	if (cfg->GetValue(CSTR("SvrPort")).SetTo(s))
	{
		s->ToUInt16(port);
	}
	NEW_CLASS(this->env, OrganWebEnv(this->sockf, 0, this->log, db, cfg->GetValue(CSTR("ImageDir")), port, 0, cfg->GetValue(CSTR("CacheDir")), cfg->GetValue(CSTR("DataDir")), scnSize, cfg->GetValue(CSTR("ReloadPwd")), 0, eng, cfg->GetValue(CSTR("OSMCacheOath"))->ToCString()));
	cfg.Delete();

	this->btnReload = ui->NewButton(*this, CSTR("&Reload"));
	this->btnReload->SetRect(40, 16, 75, 23, false);
	this->btnReload->HandleButtonClick(OnReloadClicked, this);
	if (this->env->GetErrorType() != SSWR::OrganMgr::OrganEnv::ERR_NONE)
	{
		ui->ShowMsgOK(CSTR("Error in starting server"), CSTR("Error"), this);
	}
}

SSWR::OrganMgr::OrganWebForm::~OrganWebForm()
{
	SDEL_CLASS(this->env);
	this->clif.Delete();
	this->sockf.Delete();
}

Bool SSWR::OrganMgr::OrganWebForm::IsError()
{
	if (this->env == 0)
		return true;
	if (this->env->GetErrorType() != SSWR::OrganMgr::OrganEnv::ERR_NONE)
		return true;
	return false;
}