#include "Stdafx.h"
#include "Core/Core.h"
#include "DB/DBTool.h"
#include "DB/MDBFileConn.h"
#include "DB/ODBCConn.h"
#include "IO/ConsoleWriter.h"
#include "IO/IniFile.h"
#include "Manage/ExceptionRecorder.h"
#include "Media/DrawEngineFactory.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/OrganMgr/OrganWebHandler.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	DB::DBTool *db;
	IO::ConsoleWriter *console;
	UInt32 scnSize = 0;
	Int32 unorganizedGroupId = 0;
	SSWR::OrganMgr::OrganWebHandler *dataHdlr;
	Net::SocketFactory *sockf;
	Net::SSLEngine *ssl = 0;
	IO::LogTool *log;
	Text::String *s;
	
//	MemSetBreakPoint(0x4ab9e78);
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("Error.txt"), Manage::ExceptionRecorder::EA_RESTART));
	NEW_CLASS(console, IO::ConsoleWriter());
	dataHdlr = 0;
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(log, IO::LogTool());

	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg == 0)
	{
		console->WriteLineC(UTF8STRC("Error in loading config file"));
	}
	else
	{
		if ((s = cfg->GetValue(CSTR("SSLEnable"))) != 0)
		{
			Int32 sslEnable = s->ToInt32();
			if (sslEnable)
			{
				ssl =  Net::SSLEngineFactory::Create(sockf, false);
				if (ssl == 0)
				{
					console->WriteLineC(UTF8STRC("Error in initializing SSL engine"));
				}
				else
				{
					Text::String *certFile = cfg->GetValue(CSTR("SSLCert"));
					Text::String *keyFile = cfg->GetValue(CSTR("SSLKey"));
					if (certFile == 0)
					{
						console->WriteLineC(UTF8STRC("SSLCert not found"));
						SDEL_CLASS(ssl);
					}
					else if (keyFile == 0)
					{
						console->WriteLineC(UTF8STRC("SSLKey not found"));
						SDEL_CLASS(ssl);
					}
					else if (!ssl->SetServerCerts(certFile->ToCString(), keyFile->ToCString()))
					{
						console->WriteLineC(UTF8STRC("Error in loading SSL Cert/key"));
						SDEL_CLASS(ssl);
					}
				}
			}
		}
		if ((s = cfg->GetValue(CSTR("ScreenSize"))) != 0)
		{
			scnSize = s->ToUInt32();
		}
		if (scnSize <= 0)
		{
			scnSize = 1800;
		}
		if ((s = cfg->GetValue(CSTR("Unorganized"))) != 0)
		{
			s->ToInt32(&unorganizedGroupId);
		}
		if (cfg->GetValue(CSTR("MDBFile")))
		{
			db = DB::MDBFileConn::CreateDBTool(cfg->GetValue(CSTR("MDBFile")), log, CSTR("DB: "));
		}
		else if (cfg->GetValue(CSTR("MySQLServer")))
		{
			db = Net::MySQLTCPClient::CreateDBTool(sockf, cfg->GetValue(CSTR("MySQLServer")), cfg->GetValue(CSTR("MySQLDB")), cfg->GetValue(CSTR("MySQLUID")), cfg->GetValue(CSTR("MySQLPwd")), log, CSTR("DB: "));
		}
		else
		{
			db = DB::ODBCConn::CreateDBTool(cfg->GetValue(CSTR("DBDSN")), cfg->GetValue(CSTR("DBUID")), cfg->GetValue(CSTR("DBPwd")), cfg->GetValue(CSTR("DBSchema")), log, CSTR("DB: "));
		}
		UInt16 port;
		cfg->GetValue(CSTR("SvrPort"))->ToUInt16S(&port, 0);
		NEW_CLASS(dataHdlr, SSWR::OrganMgr::OrganWebHandler(sockf, ssl, log, db, cfg->GetValue(CSTR("ImageDir")), port, cfg->GetValue(CSTR("CacheDir")), cfg->GetValue(CSTR("DataDir")), scnSize, cfg->GetValue(CSTR("ReloadPwd")), unorganizedGroupId, Media::DrawEngineFactory::CreateDrawEngine()));
		DEL_CLASS(cfg);

		if (dataHdlr->IsError())
		{
			console->WriteLineC(UTF8STRC("Error in starting server"));
		}
		else
		{
			console->WriteLineC(UTF8STRC("SOrganWeb started"));
			progCtrl->WaitForExit(progCtrl);
		}

		DEL_CLASS(dataHdlr);
	}

	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	DEL_CLASS(log);

	DEL_CLASS(console);
	DEL_CLASS(exHdlr);
	return 0;
}
