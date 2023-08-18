#include "Stdafx.h"
#include "DB/DBConfig.h"
#include "DB/MSSQLConn.h"

DB::DBTool *DB::DBConfig::LoadFromConfig(NotNullPtr<Net::SocketFactory> sockf, IO::ConfigFile *cfg, IO::LogTool *log)
{
	Text::CString logPrefix = CSTR("DB: ");
	Text::String *s = cfg->GetValue(CSTR("DBType"));
	if (s == 0)
	{
		log->LogMessage(CSTR("DBType not found in config"), IO::LogHandler::LogLevel::Error);
		return 0;
	}
	DB::DBTool *db;
	if (s->Equals(UTF8STRC("MSSQL")))
	{
		UInt16 port;
		Text::String *serverHost = cfg->GetValue(CSTR("MSSQLHost"));
		Text::String *sPort = cfg->GetValue(CSTR("MSSQLPort"));
		Text::String *sSSL = cfg->GetValue(CSTR("MSSQLEncrypt"));
		Text::String *database = cfg->GetValue(CSTR("MSSQLDatabase"));
		Text::String *userName = cfg->GetValue(CSTR("MSSQLUser"));
		Text::String *password = cfg->GetValue(CSTR("MSSQLPwd"));
		Net::SocketUtil::AddressInfo addr;
		if (serverHost == 0)
		{
			log->LogMessage(CSTR("MSSQLHost is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		else if (!sockf->DNSResolveIP(serverHost->ToCString(), addr))
		{
			log->LogMessage(CSTR("MSSQLHost is not valid"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (sPort == 0)
		{
			log->LogMessage(CSTR("MSSQLPort is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		else if (!sPort->ToUInt16(port) || port == 0)
		{
			log->LogMessage(CSTR("MSSQLPort is not valid"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (database == 0 || database->leng == 0)
		{
			log->LogMessage(CSTR("MSSQLDatabase is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (userName == 0 || userName->leng == 0)
		{
			log->LogMessage(CSTR("MSSQLUser is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (password == 0 || password->leng == 0)
		{
			log->LogMessage(CSTR("MSSQLPwd is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		db = DB::MSSQLConn::CreateDBToolTCP(serverHost->ToCString(), port, sSSL && sSSL->Equals(UTF8STRC("1")), database->ToCString(), userName->ToCString(), password->ToCString(), log, logPrefix);
		if (db == 0)
			log->LogMessage(CSTR("Error in connecting to MSSQL database"), IO::LogHandler::LogLevel::Error);
		return db;
	}
	else
	{
		log->LogMessage(CSTR("Unknown DBType in config"), IO::LogHandler::LogLevel::Error);
		return 0;
	}
}
