#include "Stdafx.h"
#include "Core/Core.h"
#include "DB/DBTool.h"
#include "DB/MDBFileConn.h"
#include "DB/ODBCConn.h"
#include "IO/ConsoleWriter.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "Manage/ExceptionRecorder.h"
#include "Media/DrawEngineFactory.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/OrganMgr/OrganWebEnv.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Manage::ExceptionRecorder exHdlr(CSTR("Error.txt"), Manage::ExceptionRecorder::EA_RESTART);
	{
		DB::DBTool *db;
		IO::ConsoleWriter console;
		UInt32 scnSize = 0;
		Int32 unorganizedGroupId = 0;
		SSWR::OrganMgr::OrganWebEnv *env;
		Net::OSSocketFactory sockf(true);
		Net::SSLEngine *ssl = 0;
		IO::LogTool log;
		Text::String *s;
		env = 0;
		Text::CString osmCacheDir;
		UTF8Char sbuff[512];
		UTF8Char *sptr;

		IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
		if (cfg == 0)
		{
			console.WriteLineC(UTF8STRC("Error in loading config file"));
		}
		else
		{
			UInt16 sslPort = 0;
			if ((s = cfg->GetValue(CSTR("SSLPort"))) != 0)
			{
				if (s->ToUInt16(&sslPort) && sslPort != 0)
				{
					ssl =  Net::SSLEngineFactory::Create(&sockf, false);
					if (ssl == 0)
					{
						console.WriteLineC(UTF8STRC("Error in initializing SSL engine"));
					}
					else
					{
						Text::String *certFile = cfg->GetValue(CSTR("SSLCert"));
						Text::String *keyFile = cfg->GetValue(CSTR("SSLKey"));
						if (certFile == 0)
						{
							console.WriteLineC(UTF8STRC("SSLCert not found"));
							SDEL_CLASS(ssl);
						}
						else if (keyFile == 0)
						{
							console.WriteLineC(UTF8STRC("SSLKey not found"));
							SDEL_CLASS(ssl);
						}
						else if (!ssl->ServerSetCerts(certFile->ToCString(), keyFile->ToCString()))
						{
							console.WriteLineC(UTF8STRC("Error in loading SSL Cert/key"));
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
				db = DB::MDBFileConn::CreateDBTool(cfg->GetValue(CSTR("MDBFile")), &log, CSTR("DB: "));
			}
			else if (cfg->GetValue(CSTR("MySQLServer")))
			{
				db = Net::MySQLTCPClient::CreateDBTool(&sockf, cfg->GetValue(CSTR("MySQLServer")), cfg->GetValue(CSTR("MySQLDB")), cfg->GetValue(CSTR("MySQLUID")), cfg->GetValue(CSTR("MySQLPwd")), &log, CSTR("DB: "));
			}
			else
			{
				db = DB::ODBCConn::CreateDBTool(cfg->GetValue(CSTR("DBDSN")), cfg->GetValue(CSTR("DBUID")), cfg->GetValue(CSTR("DBPwd")), cfg->GetValue(CSTR("DBSchema")), &log, CSTR("DB: "));
			}
			UInt16 port;
			cfg->GetValue(CSTR("SvrPort"))->ToUInt16S(&port, 0);
			s = cfg->GetValue(CSTR("OSMCacheDir"));
			if (s)
			{
				osmCacheDir = s->ToCString();
			}
			else
			{
				sptr = IO::Path::GetProcessFileName(sbuff);
				sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTile"));
				osmCacheDir = CSTRP(sbuff, sptr);
			}
			NEW_CLASS(env, SSWR::OrganMgr::OrganWebEnv(&sockf, ssl, &log, db, cfg->GetValue(CSTR("ImageDir")), port, sslPort, cfg->GetValue(CSTR("CacheDir")), cfg->GetValue(CSTR("DataDir")), scnSize, cfg->GetValue(CSTR("ReloadPwd")), unorganizedGroupId, Media::DrawEngineFactory::CreateDrawEngine(), osmCacheDir));
			DEL_CLASS(cfg);

			if (env->IsError())
			{
				console.WriteLineC(UTF8STRC("Error in starting server"));
			}
			else
			{
				console.WriteLineC(UTF8STRC("SOrganWeb started"));
				progCtrl->WaitForExit(progCtrl);
			}

			DEL_CLASS(env);
		}

		SDEL_CLASS(ssl);
	}
	return 0;
}
