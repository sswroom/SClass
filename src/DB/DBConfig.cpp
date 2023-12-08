#include "Stdafx.h"
#include "DB/DBConfig.h"
#include "DB/MSSQLConn.h"
#include "DB/PostgreSQLConn.h"

Optional<DB::DBTool> DB::DBConfig::LoadFromConfig(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<IO::ConfigFile> cfg, NotNullPtr<IO::LogTool> log)
{
	Text::CString logPrefix = CSTR("DB: ");
	NotNullPtr<Text::String> s;
	if (!cfg->GetValue(CSTR("DBType")).SetTo(s))
	{
		log->LogMessage(CSTR("DBType not found in config"), IO::LogHandler::LogLevel::Error);
		return 0;
	}
	NotNullPtr<DB::DBTool> db;
	if (s->Equals(UTF8STRC("MSSQL")))
	{
		UInt16 port;
		NotNullPtr<Text::String> serverHost;
		NotNullPtr<Text::String> sPort;
		Text::String *sSSL = cfg->GetValue(CSTR("MSSQLEncrypt")).OrNull();
		NotNullPtr<Text::String> database;
		NotNullPtr<Text::String> userName;
		NotNullPtr<Text::String> password;
		Net::SocketUtil::AddressInfo addr;
		if (!cfg->GetValue(CSTR("MSSQLHost")).SetTo(serverHost))
		{
			log->LogMessage(CSTR("MSSQLHost is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		else if (!sockf->DNSResolveIP(serverHost->ToCString(), addr))
		{
			log->LogMessage(CSTR("MSSQLHost is not valid"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetValue(CSTR("MSSQLPort")).SetTo(sPort))
		{
			log->LogMessage(CSTR("MSSQLPort is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		else if (!sPort->ToUInt16(port) || port == 0)
		{
			log->LogMessage(CSTR("MSSQLPort is not valid"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetValue(CSTR("MSSQLDatabase")).SetTo(database) || database->leng == 0)
		{
			log->LogMessage(CSTR("MSSQLDatabase is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetValue(CSTR("MSSQLUser")).SetTo(userName) || userName->leng == 0)
		{
			log->LogMessage(CSTR("MSSQLUser is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetValue(CSTR("MSSQLPwd")).SetTo(password) || password->leng == 0)
		{
			log->LogMessage(CSTR("MSSQLPwd is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!DB::MSSQLConn::CreateDBToolTCP(serverHost->ToCString(), port, sSSL && sSSL->Equals(UTF8STRC("1")), database->ToCString(), userName->ToCString(), password->ToCString(), log, logPrefix).SetTo(db))
		{
			log->LogMessage(CSTR("Error in connecting to MSSQL database"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		return db;
	}
	else if (s->Equals(UTF8STRC("PostgreSQL")))
	{
		UInt16 port;
		NotNullPtr<Text::String> serverHost;
		NotNullPtr<Text::String> sPort;
		NotNullPtr<Text::String> database;
		NotNullPtr<Text::String> userName;
		NotNullPtr<Text::String> password;
		if (!cfg->GetValue(CSTR("PSQLHost")).SetTo(serverHost))
		{
			log->LogMessage(CSTR("PSQLHost is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetValue(CSTR("PSQLPort")).SetTo(sPort))
		{
			log->LogMessage(CSTR("PSQLPort is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		else if (!sPort->ToUInt16(port) || port == 0)
		{
			log->LogMessage(CSTR("PSQLPort is not valid"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetValue(CSTR("PSQLDatabase")).SetTo(database) || database->leng == 0)
		{
			log->LogMessage(CSTR("PSQLDatabase is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetValue(CSTR("PSQLUser")).SetTo(userName) || userName->leng == 0)
		{
			log->LogMessage(CSTR("PSQLUser is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetValue(CSTR("PSQLPwd")).SetTo(password) || password->leng == 0)
		{
			log->LogMessage(CSTR("PSQLPwd is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!DB::PostgreSQLConn::CreateDBTool(serverHost->ToCString(), port, database->ToCString(), userName->ToCString(), password->ToCString(), log, logPrefix).SetTo(db))
		{
			log->LogMessage(CSTR("Error in connecting to PostgreSQL database"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		return db;
	}
	else
	{
		log->LogMessage(CSTR("Unknown DBType in config"), IO::LogHandler::LogLevel::Error);
		return 0;
	}
}
