#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBTool.h"
#include "DB/MySQLConn.h"
#include "Net/MySQLTCPClient.h"
#include "Text/StringBuilderUTF8.h"


const UTF8Char *DB::MySQLConn::GetConnServer()
{
	return this->server;
}

const UTF8Char *DB::MySQLConn::GetConnDB()
{
	return this->databse;
}

const UTF8Char *DB::MySQLConn::GetConnUID()
{
	return this->uid;
}

const UTF8Char *DB::MySQLConn::GetConnPWD()
{
	return this->pwd;
}

DB::DBTool *DB::MySQLConn::CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log)
{
	return 0;
}

DB::DBTool *DB::MySQLConn::CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log, Bool useMut)
{
	return 0;
}

DB::DBTool *DB::MySQLConn::CreateDBTool(Net::SocketFactory *sockf, const UTF8Char *serverName, const UTF8Char *dbName, const UTF8Char *uid, const UTF8Char *pwd, IO::LogTool *log, Bool useMut, const UTF8Char *logPrefix)
{
	Net::MySQLTCPClient *conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(serverName, &addr))
	{
		if (log)
		{
			Text::StringBuilderUTF8 sb;
			if (logPrefix)
			{
				sb.Append(logPrefix);
			}
			sb.Append((const UTF8Char*)"Error in resolving MySQL Server IP");
			log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERROR);
		}
		return 0;
	}
	NEW_CLASS(conn, Net::MySQLTCPClient(sockf, &addr, 3306, uid, pwd, dbName));
	if (!conn->IsError())
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, useMut, logPrefix));
		return db;
	}
	else
	{
		if (log)
		{
			Text::StringBuilderUTF8 sb;
			if (logPrefix)
			{
				sb.Append(logPrefix);
			}
			sb.Append((const UTF8Char*)"Error in connecting to MySQL Server: ");
			conn->GetErrorMsg(&sb);
			log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERROR);
		}
		DEL_CLASS(conn);
		return 0;
	}
}

DB::DBTool *DB::MySQLConn::CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log, Bool useMut, const UTF8Char *logPrefix)
{
	return 0;
}
