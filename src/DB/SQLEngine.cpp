#include "Stdafx.h"
#include "DB/SQLEngine.h"

DB::SQLEngine::SQLEngine(DB::SQLType sqlType, Text::CStringNN sourceName) : DBConn(sourceName)
{
	this->sqlType = sqlType;
	if (sqlType == DB::SQLType::MySQL)
	{
		this->tzQhr = 0;
	}
	else
	{
		this->tzQhr = Data::DateTimeUtil::GetLocalTzQhr();
	}
	this->lastErrorMsg = nullptr;
}

DB::SQLEngine::~SQLEngine()
{
	OPTSTR_DEL(this->lastErrorMsg);
}

DB::SQLType DB::SQLEngine::GetSQLType() const
{
	return this->sqlType;
}

DB::DBConn::ConnType DB::SQLEngine::GetConnType() const
{
	return DB::DBConn::ConnType::SQLEngine;
}

Int8 DB::SQLEngine::GetTzQhr() const
{
	return this->tzQhr;
}

void DB::SQLEngine::ForceTz(Int8 tzQhr)
{
	this->tzQhr = tzQhr;
}

void DB::SQLEngine::GetConnName(NN<Text::StringBuilderUTF8> sb)
{
	sb->Append(this->sourceName);
}

void DB::SQLEngine::Close()
{
}

IntOS DB::SQLEngine::ExecuteNonQuery(Text::CStringNN sql)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Support select SQL only: "));
	sb.Append(sql);
	OPTSTR_DEL(this->lastErrorMsg);
	this->lastErrorMsg = Text::String::New(sb.ToCString());
	this->lastDataError = DB::DBConn::DataError::ExecSQLError;
	return 0;
}

Optional<DB::DBReader> DB::SQLEngine::ExecuteReader(Text::CStringNN sql)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Unsupported SQL: "));
	sb.Append(sql);
	OPTSTR_DEL(this->lastErrorMsg);
	this->lastErrorMsg = Text::String::New(sb.ToCString());
	this->lastDataError = DB::DBConn::DataError::ExecSQLError;
	return nullptr;
}

Bool DB::SQLEngine::IsLastDataError()
{
	return this->lastDataError == DB::DBConn::DataError::ExecSQLError;
}

Optional<DB::TableDef> DB::SQLEngine::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	return {};
}

Optional<DB::DBTransaction> DB::SQLEngine::BeginTransaction()
{
	return nullptr;
}

void DB::SQLEngine::Commit(NN<DB::DBTransaction> tran)
{
}

void DB::SQLEngine::Rollback(NN<DB::DBTransaction> tran)
{
}
