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
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder((const UTF8Char*)"Error.txt", Manage::ExceptionRecorder::EA_RESTART));
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
		if ((s = cfg->GetValue(UTF8STRC("SSLEnable"))) != 0)
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
					Text::String *certFile = cfg->GetValue(UTF8STRC("SSLCert"));
					Text::String *keyFile = cfg->GetValue(UTF8STRC("SSLKey"));
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
		if ((s = cfg->GetValue(UTF8STRC("ScreenSize"))) != 0)
		{
			scnSize = s->ToUInt32();
		}
		if (scnSize <= 0)
		{
			scnSize = 1800;
		}
		if ((s = cfg->GetValue(UTF8STRC("Unorganized"))) != 0)
		{
			s->ToInt32(&unorganizedGroupId);
		}
		if (cfg->GetValue(UTF8STRC("MDBFile")))
		{
			db = DB::MDBFileConn::CreateDBTool(cfg->GetValue(UTF8STRC("MDBFile")), log, (const UTF8Char*)"DB: ");
		}
		else if (cfg->GetValue(UTF8STRC("MySQLServer")))
		{
			db = Net::MySQLTCPClient::CreateDBTool(sockf, cfg->GetValue(UTF8STRC("MySQLServer")), cfg->GetValue(UTF8STRC("MySQLDB")), cfg->GetValue(UTF8STRC("MySQLUID")), cfg->GetValue(UTF8STRC("MySQLPwd")), log, (const UTF8Char*)"DB: ");
		}
		else
		{
			db = DB::ODBCConn::CreateDBTool(cfg->GetValue(UTF8STRC("DBDSN")), cfg->GetValue(UTF8STRC("DBUID")), cfg->GetValue(UTF8STRC("DBPwd")), cfg->GetValue(UTF8STRC("DBSchema")), log, (const UTF8Char*)"DB: ");
		}
		UInt16 port;
		cfg->GetValue(UTF8STRC("SvrPort"))->ToUInt16S(&port, 0);
		NEW_CLASS(dataHdlr, SSWR::OrganMgr::OrganWebHandler(sockf, ssl, log, db, cfg->GetValue(UTF8STRC("ImageDir")), port, cfg->GetValue(UTF8STRC("CacheDir")), cfg->GetValue(UTF8STRC("DataDir")), scnSize, cfg->GetValue(UTF8STRC("ReloadPwd")), unorganizedGroupId, Media::DrawEngineFactory::CreateDrawEngine()));
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
