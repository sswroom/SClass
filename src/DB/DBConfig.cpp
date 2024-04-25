#include "Stdafx.h"
#include "DB/DBConfig.h"
#include "DB/MSSQLConn.h"
#include "DB/PostgreSQLConn.h"
#include "DB/SQLiteFile.h"
#include "Net/SSHManager.h"

Optional<DB::DBTool> DB::DBConfig::LoadFromConfig(NN<Net::SocketFactory> sockf, NN<IO::ConfigFile> cfg, Text::CString cfgCategory, NN<IO::LogTool> log)
{
	Text::CStringNN logPrefix = CSTR("DB: ");
	Text::CStringNN category = cfgCategory.OrEmpty();
	NN<Text::String> sshHost;
	UInt16 sshPort;
	NN<Text::String> sshUser;
	NN<Text::String> sshPassword;
	NN<Text::String> s;
	if (!cfg->GetCateValue(category, CSTR("DBType")).SetTo(s))
	{
		log->LogMessage(CSTR("DBType not found in config"), IO::LogHandler::LogLevel::Error);
		return 0;
	}
	NN<DB::DBTool> db;
	if (s->Equals(UTF8STRC("MSSQL")))
	{
		UInt16 port;
		NN<Text::String> serverHost;
		NN<Text::String> sPort;
		Text::String *sSSL = cfg->GetCateValue(category, CSTR("MSSQLEncrypt")).OrNull();
		NN<Text::String> database;
		NN<Text::String> userName;
		NN<Text::String> password;
		Net::SocketUtil::AddressInfo addr;
		if (!cfg->GetCateValue(category, CSTR("MSSQLHost")).SetTo(serverHost))
		{
			log->LogMessage(CSTR("MSSQLHost is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		else if (!sockf->DNSResolveIP(serverHost->ToCString(), addr))
		{
			log->LogMessage(CSTR("MSSQLHost is not valid"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetCateValue(category, CSTR("MSSQLPort")).SetTo(sPort))
		{
			log->LogMessage(CSTR("MSSQLPort is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		else if (!sPort->ToUInt16(port) || port == 0)
		{
			log->LogMessage(CSTR("MSSQLPort is not valid"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetCateValue(category, CSTR("MSSQLDatabase")).SetTo(database) || database->leng == 0)
		{
			log->LogMessage(CSTR("MSSQLDatabase is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetCateValue(category, CSTR("MSSQLUser")).SetTo(userName) || userName->leng == 0)
		{
			log->LogMessage(CSTR("MSSQLUser is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetCateValue(category, CSTR("MSSQLPwd")).SetTo(password) || password->leng == 0)
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
		NN<Text::String> serverHost;
		NN<Text::String> sPort;
		NN<Text::String> database;
		NN<Text::String> userName;
		NN<Text::String> password;
		if (!cfg->GetCateValue(category, CSTR("PSQLHost")).SetTo(serverHost))
		{
			log->LogMessage(CSTR("PSQLHost is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetCateValue(category, CSTR("PSQLPort")).SetTo(sPort))
		{
			log->LogMessage(CSTR("PSQLPort is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		else if (!sPort->ToUInt16(port) || port == 0)
		{
			log->LogMessage(CSTR("PSQLPort is not valid"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetCateValue(category, CSTR("PSQLDatabase")).SetTo(database) || database->leng == 0)
		{
			log->LogMessage(CSTR("PSQLDatabase is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetCateValue(category, CSTR("PSQLUser")).SetTo(userName) || userName->leng == 0)
		{
			log->LogMessage(CSTR("PSQLUser is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetCateValue(category, CSTR("PSQLPwd")).SetTo(password) || password->leng == 0)
		{
			log->LogMessage(CSTR("PSQLPwd is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (cfg->GetCateValue(category, CSTR("DBSSHHost")).SetTo(sshHost) && sshHost->leng > 0)
		{
			sshPort = 22;
			if (cfg->GetCateValue(category, CSTR("DBSSHPort")).SetTo(s) && !s->ToUInt16(sshPort))
			{
				log->LogMessage(CSTR("DBSSHPort is not valid"), IO::LogHandler::LogLevel::Error);
				return 0;
			}
			if (!cfg->GetCateValue(category, CSTR("DBSSHUser")).SetTo(sshUser))
			{
				log->LogMessage(CSTR("DBSSHUser is missing"), IO::LogHandler::LogLevel::Error);
				return 0;
			}
			if (cfg->GetCateValue(category, CSTR("DBSSHPassword")).SetTo(sshPassword))
			{
				NN<Net::SSHManager> ssh;
				NN<Net::SSHClient> cli;
				NN<Net::SSHForwarder> fwd;
				NEW_CLASSNN(ssh, Net::SSHManager(sockf));
				if (ssh->IsError())
				{
					ssh.Delete();
					log->LogMessage(CSTR("Error in initializing SSH Engine"), IO::LogHandler::LogLevel::Error);
					return 0;
				}
				if (!ssh->CreateClient(sshHost->ToCString(), sshPort, sshUser->ToCString(), sshPassword->ToCString()).SetTo(cli))
				{
					ssh.Delete();
					log->LogMessage(CSTR("Error in initializing SSH tunnel"), IO::LogHandler::LogLevel::Error);
					return 0;
				}
				if (!cli->CreateForward(0, serverHost->ToCString(), port).SetTo(fwd))
				{
					cli.Delete();
					ssh.Delete();
					log->LogMessage(CSTR("Error in starting tunnel"), IO::LogHandler::LogLevel::Error);
					return 0;
				}
				port = fwd->GetListenPort();
				if (!DB::PostgreSQLConn::CreateDBTool(CSTR("127.0.0.1"), port, database->ToCString(), userName->ToCString(), password->ToCString(), log, logPrefix).SetTo(db))
				{
					cli.Delete();
					ssh.Delete();
					log->LogMessage(CSTR("Error in connecting to database"), IO::LogHandler::LogLevel::Error);
					return 0;
				}
				db->SetSSHTunnel(ssh, cli);
				return db;
			}
			else
			{
				log->LogMessage(CSTR("DBSSHPassword is missing"), IO::LogHandler::LogLevel::Error);
				return 0;
			}
		}
		else if (!DB::PostgreSQLConn::CreateDBTool(serverHost->ToCString(), port, database->ToCString(), userName->ToCString(), password->ToCString(), log, logPrefix).SetTo(db))
		{
			log->LogMessage(CSTR("Error in connecting to PostgreSQL database"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		return db;
	}
	else if (s->Equals(UTF8STRC("SQLite")))
	{
		NN<Text::String> filePath;
		if (!cfg->GetCateValue(category, CSTR("SQLiteFile")).SetTo(filePath))
		{
			log->LogMessage(CSTR("SQLiteFile is missing"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!DB::SQLiteFile::CreateDBTool(filePath, log, logPrefix).SetTo(db))
		{
			log->LogMessage(CSTR("Error in opening to SQLite database"), IO::LogHandler::LogLevel::Error);
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
