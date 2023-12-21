#include "Stdafx.h"
#include "Core/Core.h"
#include "DB/DBTool.h"
#include "DB/MDBFileConn.h"
#include "DB/ODBCConn.h"
#include "IO/ConsoleWriter.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "IO/WriterLogHandler.h"
#include "Manage/ExceptionRecorder.h"
#include "Media/DrawEngineFactory.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/OrganWeb/OrganWebEnv.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Manage::ExceptionRecorder exHdlr(CSTR("Error.txt"), Manage::ExceptionRecorder::EA_RESTART);
	{
		DB::DBTool *db;
		IO::ConsoleWriter console;
		UInt32 scnSize = 0;
		Int32 unorganizedGroupId = 0;
		Net::OSSocketFactory sockf(true);
		Optional<Net::SSLEngine> ssl = 0;
		IO::LogTool log;
		NotNullPtr<Text::String> s;
		Text::CString osmCacheDir;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		IO::WriterLogHandler printLog(&console, false);
		log.AddLogHandler(printLog, IO::LogHandler::LogLevel::ErrorDetail);

		IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
		if (cfg == 0)
		{
			console.WriteLineC(UTF8STRC("Error in loading config file"));
		}
		else
		{
			UInt16 sslPort = 0;
			if (cfg->GetValue(CSTR("SSLPort")).SetTo(s))
			{
				if (s->ToUInt16(sslPort) && sslPort != 0)
				{
					ssl = Net::SSLEngineFactory::Create(sockf, false);
					NotNullPtr<Net::SSLEngine> nnssl;
					if (!ssl.SetTo(nnssl))
					{
						console.WriteLineC(UTF8STRC("Error in initializing SSL engine"));
					}
					else
					{
						NotNullPtr<Text::String> certFile;
						NotNullPtr<Text::String> keyFile;
						if (!cfg->GetValue(CSTR("SSLCert")).SetTo(certFile))
						{
							console.WriteLineC(UTF8STRC("SSLCert not found"));
							ssl.Delete();
						}
						else if (!cfg->GetValue(CSTR("SSLKey")).SetTo(keyFile))
						{
							console.WriteLineC(UTF8STRC("SSLKey not found"));
							ssl.Delete();
						}
						else if (!nnssl->ServerSetCerts(certFile->ToCString(), keyFile->ToCString()))
						{
							console.WriteLineC(UTF8STRC("Error in loading SSL Cert/key"));
							ssl.Delete();
						}
					}
				}
			}
			if (cfg->GetValue(CSTR("ScreenSize")).SetTo(s))
			{
				scnSize = s->ToUInt32();
			}
			if (scnSize <= 0)
			{
				scnSize = 1800;
			}
			if (cfg->GetValue(CSTR("Unorganized")).SetTo(s))
			{
				s->ToInt32(unorganizedGroupId);
			}
			if (cfg->GetValue(CSTR("MDBFile")).SetTo(s))
			{
				db = DB::MDBFileConn::CreateDBTool(s, log, CSTR("DB: "));
			}
			else if (cfg->GetValue(CSTR("MySQLServer")).SetTo(s))
			{
				db = Net::MySQLTCPClient::CreateDBTool(sockf, s, cfg->GetValue(CSTR("MySQLDB")), Text::String::OrEmpty(cfg->GetValue(CSTR("MySQLUID"))), Text::String::OrEmpty(cfg->GetValue(CSTR("MySQLPwd"))), log, CSTR("DB: "));
			}
			else if (cfg->GetValue(CSTR("DBDSN")).SetTo(s))
			{
				db = DB::ODBCConn::CreateDBTool(s, cfg->GetValue(CSTR("DBUID")), cfg->GetValue(CSTR("DBPwd")), cfg->GetValue(CSTR("DBSchema")), log, CSTR("DB: ")).OrNull();
			}
			else
			{
				db = 0;
			}
			UInt16 port;
			if (!cfg->GetValue(CSTR("SvrPort")).SetTo(s) || !s->ToUInt16(port))
				port = 0;
			if (cfg->GetValue(CSTR("OSMCacheDir")).SetTo(s))
			{
				osmCacheDir = s->ToCString();
			}
			else
			{
				sptr = IO::Path::GetProcessFileName(sbuff);
				sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTile"));
				osmCacheDir = CSTRP(sbuff, sptr);
			}
			NotNullPtr<Text::String> imageDir;
			NotNullPtr<Text::String> dataDir;
			if (!cfg->GetValue(CSTR("ImageDir")).SetTo(imageDir))
			{
				console.WriteLineC(UTF8STRC("Config ImageDir not found"));
			}
			else if (!cfg->GetValue(CSTR("DataDir")).SetTo(dataDir))
			{
				console.WriteLineC(UTF8STRC("Config DataDir not found"));
			}
			else
			{
				SSWR::OrganWeb::OrganWebEnv env(sockf, ssl, log, db, imageDir, port, sslPort, cfg->GetValue(CSTR("CacheDir")), dataDir, scnSize, cfg->GetValue(CSTR("ReloadPwd")), unorganizedGroupId, Media::DrawEngineFactory::CreateDrawEngine(), osmCacheDir);

				if (env.IsError())
				{
					console.WriteLineC(UTF8STRC("Error in starting server"));
				}
				else
				{
					console.WriteLineC(UTF8STRC("SOrganWeb started"));
					progCtrl->WaitForExit(progCtrl);
				}
			}
			DEL_CLASS(cfg);
		}

		ssl.Delete();
		log.RemoveLogHandler(printLog);
	}
	return 0;
}
