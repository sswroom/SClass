#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBTool.h"
#include "DB/MySQLConn.h"
#include "Net/MySQLTCPClient.h"
#include "Text/StringBuilderUTF8.h"


NN<Text::String> DB::MySQLConn::GetConnServer()
{
	return this->server;
}

Optional<Text::String> DB::MySQLConn::GetConnDB()
{
	return this->database;
}

Optional<Text::String> DB::MySQLConn::GetConnUID()
{
	return this->uid;
}

Optional<Text::String> DB::MySQLConn::GetConnPWD()
{
	return this->pwd;
}

/*DB::DBTool *DB::MySQLConn::CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log)
{
	return 0;
}*/

Optional<DB::DBTool> DB::MySQLConn::CreateDBTool(NN<Net::TCPClientFactory> clif, NN<Text::String> serverName, Optional<Text::String> dbName, NN<Text::String> uid, NN<Text::String> pwd, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<Net::MySQLTCPClient> conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (!clif->GetSocketFactory()->DNSResolveIP(serverName->ToCString(), addr))
	{
		if (log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendOpt(logPrefix);
			sb.AppendC(UTF8STRC("Error in resolving MySQL Server IP"));
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		return nullptr;
	}
	NEW_CLASSNN(conn, Net::MySQLTCPClient(clif, addr, 3306, uid, pwd, dbName));
	if (!conn->IsError())
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		if (log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendOpt(logPrefix);
			sb.AppendC(UTF8STRC("Error in connecting to MySQL Server: "));
			conn->GetLastErrorMsg(sb);
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		conn.Delete();
		return nullptr;
	}
}

Optional<DB::DBTool> DB::MySQLConn::CreateDBTool(NN<Net::TCPClientFactory> clif, Text::CStringNN serverName, Text::CString dbName, Text::CStringNN uid, Text::CStringNN pwd, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<Net::MySQLTCPClient> conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (!clif->GetSocketFactory()->DNSResolveIP(serverName, addr))
	{
		if (log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendOpt(logPrefix);
			sb.AppendC(UTF8STRC("Error in resolving MySQL Server IP"));
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		return nullptr;
	}
	NEW_CLASSNN(conn, Net::MySQLTCPClient(clif, addr, 3306, uid, pwd, dbName));
	if (!conn->IsError())
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		if (log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendOpt(logPrefix);
			sb.AppendC(UTF8STRC("Error in connecting to MySQL Server: "));
			conn->GetLastErrorMsg(sb);
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		conn.Delete();
		return nullptr;
	}
}

/*DB::DBTool *DB::MySQLConn::CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log, Text::CString logPrefix)
{
	return 0;
}*/
