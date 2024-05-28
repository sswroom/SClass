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

Optional<DB::DBTool> DB::MySQLConn::CreateDBTool(NN<Net::SocketFactory> sockf, NN<Text::String> serverName, Text::String *dbName, Text::String *uid, Text::String *pwd, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<Net::MySQLTCPClient> conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(serverName->ToCString(), addr))
	{
		if (log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			if (logPrefix.leng > 0)
			{
				sb.Append(logPrefix);
			}
			sb.AppendC(UTF8STRC("Error in resolving MySQL Server IP"));
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		return 0;
	}
	NEW_CLASSNN(conn, Net::MySQLTCPClient(sockf, addr, 3306, Text::String::OrEmpty(uid), Text::String::OrEmpty(pwd), dbName));
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
			if (logPrefix.leng > 0)
			{
				sb.Append(logPrefix);
			}
			sb.AppendC(UTF8STRC("Error in connecting to MySQL Server: "));
			conn->GetLastErrorMsg(sb);
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		conn.Delete();
		return 0;
	}
}

Optional<DB::DBTool> DB::MySQLConn::CreateDBTool(NN<Net::SocketFactory> sockf, Text::CStringNN serverName, Text::CString dbName, Text::CString uid, Text::CString pwd, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<Net::MySQLTCPClient> conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(serverName, addr))
	{
		if (log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			if (logPrefix.leng > 0)
			{
				sb.Append(logPrefix);
			}
			sb.AppendC(UTF8STRC("Error in resolving MySQL Server IP"));
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		return 0;
	}
	NEW_CLASSNN(conn, Net::MySQLTCPClient(sockf, addr, 3306, uid, pwd, dbName));
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
			if (logPrefix.leng > 0)
			{
				sb.Append(logPrefix);
			}
			sb.AppendC(UTF8STRC("Error in connecting to MySQL Server: "));
			conn->GetLastErrorMsg(sb);
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		conn.Delete();
		return 0;
	}
}

/*DB::DBTool *DB::MySQLConn::CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log, Text::CString logPrefix)
{
	return 0;
}*/
