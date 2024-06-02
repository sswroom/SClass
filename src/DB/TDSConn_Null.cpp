#include "Stdafx.h"
#include "DB/TDSConn.h"

DB::TDSConn::TDSConn(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, NN<IO::LogTool> log, Text::StringBuilderUTF8 *errMsg) : DBConn(serverHost)
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

NN<Text::String> DB::TDSConn::GetConnHost() const
{
	return Text::String::NewEmpty();
}

Optional<Text::String> DB::TDSConn::GetConnDB() const
{
	return 0;
}

NN<Text::String> DB::TDSConn::GetConnUID() const
{
	return Text::String::NewEmpty();
}

NN<Text::String> DB::TDSConn::GetConnPWD() const
{
	return Text::String::NewEmpty();
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

void DB::TDSConn::GetConnName(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("TDS:"));
}

void DB::TDSConn::Close()
{
}

OSInt DB::TDSConn::ExecuteNonQuery(Text::CStringNN sql)
{
	return -2;
}

Optional<DB::DBReader> DB::TDSConn::ExecuteReader(Text::CStringNN sql)
{
	return 0;
}

void DB::TDSConn::CloseReader(NN<DBReader> r)
{
}

void DB::TDSConn::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

Bool DB::TDSConn::IsLastDataError()
{
	return false;
}

void DB::TDSConn::Reconnect()
{
}

Optional<DB::DBTransaction> DB::TDSConn::BeginTransaction()
{
	return 0;
}

void DB::TDSConn::Commit(NN<DB::DBTransaction> tran)
{
}

void DB::TDSConn::Rollback(NN<DB::DBTransaction> tran)
{
}

UOSInt DB::TDSConn::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	return 0;
}

Optional<DB::DBReader> DB::TDSConn::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	return 0;
}
