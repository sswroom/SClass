#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Data/DateTime.h"
#include "DB/DBConn.h"
#include "DB/ODBCConn.h"
#include "DB/DBTool.h"
#include "Math/Math_C.h"
#include "Math/Geometry/Point.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

void DB::ODBCConn::UpdateConnInfo()
{
}

Bool DB::ODBCConn::Connect(Text::String *dsn, Text::String *uid, Text::String *pwd, Text::String *schema)
{
	this->sqlType = DB::SQLType::Unknown;
	return false;
}

Bool DB::ODBCConn::Connect(Text::CString connStr)
{
	if (this->connStr)
	{
		this->connStr->Release();
	}
	this->connStr = Text::String::New(connStr).Ptr();
	this->connErr = CE_NOT_CONNECT;
	return false;
}

DB::ODBCConn::ODBCConn(Text::CStringNN sourceName, IO::LogTool *log) : DB::DBConn(sourceName)
{
	connHand = 0;
	envHand = 0;
	lastStmtHand = 0;
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
	this->forceTz = false;
	this->axisAware = false;
}

DB::ODBCConn::ODBCConn(Text::CString connStr, Text::CStringNN sourceName, IO::LogTool *log) : DB::DBConn(sourceName)
{
	this->connHand = 0;
	this->envHand = 0;
	this->lastStmtHand = 0;
	this->log = log;
	this->connStr = 0;
	this->dsn = 0;
	this->uid = 0;
	this->pwd = 0;
	this->schema = 0;
	this->tzQhr = 0;
	this->forceTz = false;
	this->axisAware = false;
	this->Connect(connStr);
}

DB::ODBCConn::ODBCConn(Text::CStringNN dsn, Text::CString uid, Text::CString pwd, Text::CString schema, IO::LogTool *log) : DB::DBConn(dsn)
{
	this->log = log;
	this->connStr = 0;
	this->tzQhr = 0;
	this->lastStmtHand = 0;
	this->connErr = DB::ODBCConn::CE_NOT_CONNECT;
	this->dsn = Text::String::NewOrNull(dsn);
	this->uid = Text::String::NewOrNull(uid);
	this->pwd = Text::String::NewOrNull(pwd);
	this->schema = Text::String::NewOrNull(schema);
	this->forceTz = false;
	this->axisAware = false;
	this->Connect(this->dsn, this->uid, this->pwd, this->schema);
}

DB::ODBCConn::ODBCConn(NN<Text::String> dsn, Text::String *uid, Text::String *pwd, Text::String *schema, IO::LogTool *log) : DB::DBConn(dsn)
{
	this->log = log;
	this->connStr = 0;
	this->tzQhr = 0;
	this->lastStmtHand = 0;
	this->connErr = DB::ODBCConn::CE_NOT_CONNECT;
	this->dsn = dsn->Clone().Ptr();
	this->uid = uid->Clone().Ptr();
	this->pwd = pwd->Clone().Ptr();
	this->schema = schema->Clone().Ptr();
	this->forceTz = false;
	this->axisAware = false;
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
}

DB::SQLType DB::ODBCConn::GetSQLType() const
{
	return this->sqlType;
}

Bool DB::ODBCConn::IsAxisAware() const
{
	return this->axisAware;
}

DB::DBConn::ConnType DB::ODBCConn::GetConnType() const
{
	return CT_ODBC;
}

Int8 DB::ODBCConn::GetTzQhr() const
{
	return this->tzQhr;
}

void DB::ODBCConn::ForceTz(Int8 tzQhr)
{
	this->forceTz = true;
	this->tzQhr = tzQhr;
}

void DB::ODBCConn::GetConnName(NN<Text::StringBuilderUTF8> sb)
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
			sb->AppendUTF8Char('/');
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

OSInt DB::ODBCConn::ExecuteNonQuery(Text::CString sql)
{
	this->lastDataError = DB::DBConn::DE_CONN_ERROR;
	return -2;
}

/*OSInt DB::ODBCConn::ExecuteNonQuery(const WChar *sql)
{
	this->lastDataError = DB::DBConn::DE_CONN_ERROR;
	return -2;
}*/

DB::DBReader *DB::ODBCConn::ExecuteReader(Text::CString sql)
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

void DB::ODBCConn::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
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

DB::DBReader *DB::ODBCConn::GetTablesInfo(Text::CString schemaName)
{
	this->lastDataError = DB::DBConn::DE_CONN_ERROR;
	return 0;
}

UOSInt DB::ODBCConn::QueryTableNames(Text::CString schemaName, Data::ArrayListStringNN *names)
{
	return 0;
}

DB::DBReader *DB::ODBCConn::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return 0;
}

void DB::ODBCConn::ShowSQLError(const UTF16Char *state, const UTF16Char *errMsg)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("ODBC Error: ["));
	NN<Text::String> s = Text::String::NewNotNull(state);
	sb.Append(s);
	s->Release();
	sb.AppendC(UTF8STRC("] "));
	s = Text::String::NewNotNull(errMsg);
	sb.Append(s);
	s->Release();
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::ErrorDetail);
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

UOSInt DB::ODBCConn::GetDriverList(Data::ArrayListStringNN *driverList)
{
	return 0;
}

IO::ConfigFile *DB::ODBCConn::GetDriverInfo(Text::CString driverName)
{
	return 0;
}

DB::DBTool *DB::ODBCConn::CreateDBTool(NN<Text::String> dsn, Text::String *uid, Text::String *pwd, Text::String *schema, IO::LogTool *log, Text::CString logPrefix)
{
	return 0;
}

DB::DBTool *DB::ODBCConn::CreateDBTool(Text::CStringNN dsn, Text::CString uid, Text::CString pwd, Text::CString schema, IO::LogTool *log, Text::CString logPrefix)
{
	return 0;
}
