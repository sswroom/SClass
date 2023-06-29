#include "Stdafx.h"
#include "DB/TDSConn.h"

DB::TDSConn::TDSConn(Text::CString serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, IO::LogTool *log, Text::StringBuilderUTF8 *errMsg) : DBConn(serverHost)
{
	this->sqlType = SQLType::MSSQL;
	this->clsData = 0;
}

DB::TDSConn::~TDSConn()
{
}

Bool DB::TDSConn::IsConnected() const
{
	return false;
}

Text::String *DB::TDSConn::GetConnHost() const
{
	return 0;
}

Text::String *DB::TDSConn::GetConnDB() const
{
	return 0;
}

Text::String *DB::TDSConn::GetConnUID() const
{
	return 0;
}

Text::String *DB::TDSConn::GetConnPWD() const
{
	return 0;
}

DB::SQLType DB::TDSConn::GetSQLType() const
{
	return this->sqlType;
}

DB::DBConn::ConnType DB::TDSConn::GetConnType() const
{
	return CT_TDSCONN;
}

Int8 DB::TDSConn::GetTzQhr() const
{
	return Data::DateTimeUtil::GetLocalTzQhr();
}

void DB::TDSConn::ForceTz(Int8 tzQhr)
{
}

void DB::TDSConn::GetConnName(Text::StringBuilderUTF8 *sb)
{
	sb->AppendC(UTF8STRC("TDS:"));
}

void DB::TDSConn::Close()
{
}

OSInt DB::TDSConn::ExecuteNonQuery(Text::CString sql)
{
	return -2;
}

DB::DBReader *DB::TDSConn::ExecuteReader(Text::CString sql)
{
	return 0;
}

void DB::TDSConn::CloseReader(DBReader *r)
{
}

void DB::TDSConn::GetLastErrorMsg(Text::StringBuilderUTF8 *str)
{
}

Bool DB::TDSConn::IsLastDataError()
{
	return false;
}

void DB::TDSConn::Reconnect()
{
}

void *DB::TDSConn::BeginTransaction()
{
	return 0;
}

void DB::TDSConn::Commit(void *tran)
{
}

void DB::TDSConn::Rollback(void *tran)
{
}

UOSInt DB::TDSConn::QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names)
{
	return 0;
}

DB::DBReader *DB::TDSConn::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return 0;
}
