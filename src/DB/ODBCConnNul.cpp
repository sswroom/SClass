#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "DB/DBConn.h"
#include "DB/ODBCConn.h"
#include "DB/DBTool.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

void DB::ODBCConn::UpdateConnInfo()
{
}

Bool DB::ODBCConn::Connect(Text::String *dsn, Text::String *uid, Text::String *pwd, Text::String *schema)
{
	this->svrType = DB::DBUtil::ServerType::Unknown;
	return false;
}

Bool DB::ODBCConn::Connect(const UTF8Char *connStr)
{
	if (this->connStr)
	{
		this->connStr->Release();
	}
	this->connStr = Text::String::NewNotNull(connStr);
	this->connErr = CE_NOT_CONNECT;
	return false;
}

DB::ODBCConn::ODBCConn(const UTF8Char *sourceName, IO::LogTool *log) : DB::DBConn(sourceName)
{
	connHand = 0;
	envHand = 0;
	lastStmtHand = 0;
	this->tableNames = 0;
	this->log = log;
	this->connErr = CE_NOT_CONNECT;
	this->lastErrorMsg = 0;
	this->connStr = 0;
	this->connHand = 0;
	this->envHand = 0;
	this->dsn = 0;
	this->uid = 0;
	this->pwd = 0;
	this->schema = 0;
	this->enableDebug = false;
	this->tzQhr = 0;
}

DB::ODBCConn::ODBCConn(const UTF8Char *connStr, const UTF8Char *sourceName, IO::LogTool *log) : DB::DBConn(sourceName)
{
	this->connHand = 0;
	this->envHand = 0;
	this->lastStmtHand = 0;
	this->tableNames = 0;
	this->log = log;
	this->connStr = 0;
	this->dsn = 0;
	this->uid = 0;
	this->pwd = 0;
	this->schema = 0;
	this->tzQhr = 0;
	this->Connect(connStr);
}

DB::ODBCConn::ODBCConn(const UTF8Char *dsn, const UTF8Char *uid, const UTF8Char *pwd, const UTF8Char *schema, IO::LogTool *log) : DB::DBConn(dsn)
{
	this->log = log;
	this->tableNames = 0;
	this->connStr = 0;
	this->tzQhr = 0;
	this->lastStmtHand = 0;
	this->connErr = DB::ODBCConn::CE_NOT_CONNECT;
	this->dsn = Text::String::NewOrNull(dsn);
	this->uid = Text::String::NewOrNull(uid);
	this->pwd = Text::String::NewOrNull(pwd);
	this->schema = Text::String::NewOrNull(schema);
	this->Connect(this->dsn, this->uid, this->pwd, this->schema);
}

/*DB::ODBCConn::ODBCConn(const WChar *dsn, const WChar *uid, const WChar *pwd, const WChar *schema, IO::LogTool *log) : DB::DBConn(0)
{
	this->connHand = 0;
	this->envHand = 0;
	this->lastStmtHand = 0;
	this->tableNames = 0;
	this->log = log;
	this->connStr = 0;
	if (dsn)
		this->dsn = Text::StrToUTF8New(dsn);
	else
		this->dsn = 0;
	if (uid)
		this->uid = Text::StrToUTF8New(uid);
	else
		this->uid = 0;
	if (pwd)
		this->pwd = Text::StrToUTF8New(pwd);
	else
		this->pwd = 0;
	if (schema)
		this->schema = Text::StrToUTF8New(schema);
	else
		this->schema = 0;
	this->Connect(this->dsn, this->uid, this->pwd, this->schema);
}*/

DB::ODBCConn::~ODBCConn()
{
	Close();
	SDEL_STRING(this->dsn);
	SDEL_STRING(this->uid);
	SDEL_STRING(this->pwd);
	SDEL_STRING(this->connStr);
	if (this->tableNames)
	{
		OSInt i = this->tableNames->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->tableNames->GetItem(i));
		}
		DEL_CLASS(this->tableNames);
		this->tableNames = 0;
	}

}

DB::DBUtil::ServerType DB::ODBCConn::GetSvrType()
{
	return this->svrType;
}

DB::DBConn::ConnType DB::ODBCConn::GetConnType()
{
	return CT_ODBC;
}

Int8 DB::ODBCConn::GetTzQhr()
{
	return this->tzQhr;
}

void DB::ODBCConn::ForceTz(Int8 tzQhr)
{
	this->forceTz = true;
	this->tzQhr = tzQhr;
}

void DB::ODBCConn::GetConnName(Text::StringBuilderUTF *sb)
{
	sb->AppendC(UTF8STRC("ODBC:"));
	if (this->connStr)
	{
		sb->Append(this->connStr);
	}
	else if (this->dsn)
	{
		sb->Append(this->dsn);
		if (this->schema)
		{
			sb->AppendChar('/', 1);
			sb->Append(this->schema);
		}
	}
}

void DB::ODBCConn::Close()
{
}

void DB::ODBCConn::Dispose()
{
	delete this;
}

OSInt DB::ODBCConn::ExecuteNonQuery(const UTF8Char *sql)
{
	this->lastDataError = DB::DBConn::DE_CONN_ERROR;
	return -2;
}

/*OSInt DB::ODBCConn::ExecuteNonQuery(const WChar *sql)
{
	this->lastDataError = DB::DBConn::DE_CONN_ERROR;
	return -2;
}*/

DB::DBReader *DB::ODBCConn::ExecuteReader(const UTF8Char *sql)
{
	this->lastDataError = DB::DBConn::DE_CONN_ERROR;
	return 0;
}

/*DB::DBReader *DB::ODBCConn::ExecuteReader(const WChar *sql)
{
	this->lastDataError = DB::DBConn::DE_CONN_ERROR;
	return 0;
}*/

void DB::ODBCConn::CloseReader(DB::DBReader *r)
{
}

void DB::ODBCConn::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

Bool DB::ODBCConn::IsLastDataError()
{
	return false;
}

void DB::ODBCConn::Reconnect()
{
}

void *DB::ODBCConn::BeginTransaction()
{
	return 0;
}

void DB::ODBCConn::Commit(void *tran)
{
}

void DB::ODBCConn::Rollback(void *tran)
{
}

DB::ODBCConn::ConnError DB::ODBCConn::GetConnError()
{
	return this->connErr;
}

void DB::ODBCConn::SetEnableDebug(Bool enableDebug)
{

}

void DB::ODBCConn::SetTraceFile(const WChar *fileName)
{
}

UTF8Char *DB::ODBCConn::ShowTablesCmd(UTF8Char *sqlstr)
{
	return 0;
}

DB::DBReader *DB::ODBCConn::GetTablesInfo()
{
	this->lastDataError = DB::DBConn::DE_CONN_ERROR;
	return 0;
}

UOSInt DB::ODBCConn::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	return 0;
}

DB::DBReader *DB::ODBCConn::GetTableData(const UTF8Char *name, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	return 0;
}

void DB::ODBCConn::ShowSQLError(const UInt16 *state, const UInt16 *errMsg)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("ODBC Error: ["));
	const UTF8Char *csptr = Text::StrToUTF8New(state);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	sb.AppendC(UTF8STRC("] "));
	csptr = Text::StrToUTF8New(errMsg);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
}

void DB::ODBCConn::LogSQLError(void *hStmt)
{
}

Text::String *DB::ODBCConn::GetConnStr()
{
	return this->connStr;
}

Text::String *DB::ODBCConn::GetConnDSN()
{
	return this->dsn;
}

Text::String *DB::ODBCConn::GetConnUID()
{
	return this->uid;
}

Text::String *DB::ODBCConn::GetConnPWD()
{
	return this->pwd;
}

Text::String *DB::ODBCConn::GetConnSchema()
{
	return this->schema;
}

DB::DBTool *DB::ODBCConn::CreateDBTool(Text::String *dsn, Text::String *uid, Text::String *pwd, Text::String *schema, IO::LogTool *log, const UTF8Char *logPrefix)
{
	return 0;
}

DB::DBTool *DB::ODBCConn::CreateDBTool(const UTF8Char *dsn, const UTF8Char *uid, const UTF8Char *pwd, const UTF8Char *schema, IO::LogTool *log, const UTF8Char *logPrefix)
{
	return 0;
}
