#include "Stdafx.h"
#include "DB/SQL/SQLCommand.h"
#include "DB/SQL/SQLEmbeddedReader.h"
#include "DB/SQL/SQLEngine.h"
#include "DB/SQL/SQLEngineDBTable.h"
#include "DB/SQL/SQLStringReader.h"
#include "IO/Path.h"

#define VERBOSE

DB::SQL::SQLEngine::SQLEngine(DB::SQLType sqlType, Text::CStringNN sourceName) : DBConn(sourceName)
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
	Text::StringBuilderUTF8 sb;
	UIntOS i = sourceName.LastIndexOf(IO::Path::PATH_SEPERATOR);
	sb.AppendC(sourceName.v + i + 1, sourceName.leng - i - 1);
	i = sb.LastIndexOf('.');
	if (i != INVALID_INDEX)
	{
		sb.TrimToLength(i);
	}
	this->dbName = Text::String::New(sb.ToCString());
}

DB::SQL::SQLEngine::~SQLEngine()
{
	OPTSTR_DEL(this->lastErrorMsg);
	this->dbName->Release();
}

UIntOS DB::SQL::SQLEngine::QuerySchemaNames(NN<Data::ArrayListStringNN> names)
{
	NN<Text::String> s;
	UIntOS initCnt = names->GetCount();
	UIntOS i = 0;
	UIntOS j = this->tables.GetCount();
	while (i < j)
	{
		if (this->tables.GetKey(i).SetTo(s))
		{
			names->Add(s->Clone());
		}
		i++;
	}
	return names->GetCount() - initCnt;
}

UIntOS DB::SQL::SQLEngine::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	NN<Data::FastStringMapNN<SQLEngineTable>> tableMap;
	if (this->tables.GetC(schemaName.OrEmpty()).SetTo(tableMap))
	{
		NN<Text::String> s;
		UIntOS initCnt = names->GetCount();
		UIntOS i = 0;
		UIntOS j = tableMap->GetCount();
		while (i < j)
		{
			if (tableMap->GetKey(i).SetTo(s))
			{
				names->Add(s->Clone());
			}
			i++;
		}
		return names->GetCount() - initCnt;
	}
	return 0;
}

Optional<DB::DBReader> DB::SQL::SQLEngine::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<SQLEngineTable> table;
	NN<Data::FastStringMapNN<SQLEngineTable>> tableMap;
	if (this->tables.GetC(schemaName.OrEmpty()).SetTo(tableMap) && tableMap->GetC(tableName).SetTo(table))
	{
		NN<DB::DBReader> r;
		if (table->QueryTableData(colNames, dataOfst, maxCnt, ordering, condition).SetTo(r))
		{
			NN<SQLEngineReader> reader;
			NEW_CLASSNN(reader, SQLEmbeddedReader(r, table));
			return reader;
		}
		return nullptr;
	}
	return nullptr;
}

Optional<DB::TableDef> DB::SQL::SQLEngine::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<SQLEngineTable> table;
	NN<Data::FastStringMapNN<SQLEngineTable>> tableMap;
	if (this->tables.GetC(schemaName.OrEmpty()).SetTo(tableMap) && tableMap->GetC(tableName).SetTo(table))
	{
		return table->GetTableDef();
	}
	return nullptr;
}

void DB::SQL::SQLEngine::CloseReader(NN<DB::DBReader> r)
{
	NN<DB::SQL::SQLEngineReader> reader = NN<DB::SQL::SQLEngineReader>::ConvertFrom(r);
	reader.Delete();
}

void DB::SQL::SQLEngine::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	str->AppendOpt(this->lastErrorMsg);
}

void DB::SQL::SQLEngine::Reconnect()
{
}

Int8 DB::SQL::SQLEngine::GetTzQhr() const
{
	return this->tzQhr;
}

void DB::SQL::SQLEngine::ForceTzQhr(Int8 tzQhr)
{
	this->tzQhr = tzQhr;
}

UIntOS DB::SQL::SQLEngine::GetDatabaseNames(NN<Data::ArrayListStringNN> arr)
{
	arr->Add(this->sourceName->Clone());
	return 1;
}

void DB::SQL::SQLEngine::ReleaseDatabaseNames(NN<Data::ArrayListStringNN> arr)
{
	arr->FreeAll();
}

Bool DB::SQL::SQLEngine::ChangeDatabase(Text::CStringNN databaseName)
{
	return false;
}

Optional<Text::String> DB::SQL::SQLEngine::GetCurrDBName()
{
	return this->dbName;
}

DB::SQLType DB::SQL::SQLEngine::GetSQLType() const
{
	return this->sqlType;
}

DB::DBConn::ConnType DB::SQL::SQLEngine::GetConnType() const
{
	return DB::DBConn::ConnType::SQLEngine;
}

void DB::SQL::SQLEngine::GetConnName(NN<Text::StringBuilderUTF8> sb)
{
	sb->Append(this->sourceName);
}

void DB::SQL::SQLEngine::Close()
{
}

IntOS DB::SQL::SQLEngine::ExecuteNonQuery(Text::CStringNN sql)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Support select SQL only: "));
	sb.Append(sql);
	OPTSTR_DEL(this->lastErrorMsg);
	this->lastErrorMsg = Text::String::New(sb.ToCString());
	this->lastDataError = DB::DBConn::DataError::ExecSQLError;
#if defined(VERBOSE)
	printf("Unsupported SQL: %s\r\n", sql.v.Ptr());
#endif
	return 0;
}

Optional<DB::DBReader> DB::SQL::SQLEngine::ExecuteReader(Text::CStringNN sql)
{
	NN<DB::SQL::SQLCommand> cmd;
	if (!DB::SQL::SQLCommand::Parse(sql.v, this->sqlType).SetTo(cmd))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Unsupported SQL: "));
		sb.Append(sql);
		OPTSTR_DEL(this->lastErrorMsg);
		this->lastErrorMsg = Text::String::New(sb.ToCString());
		this->lastDataError = DB::DBConn::DataError::ExecSQLError;
#if defined(VERBOSE)
		printf("Unsupported SQL: %s\r\n", sql.v.Ptr());
#endif
		return nullptr;
	}
	if (cmd->GetCommandType() == DB::SQL::CommandType::ShowDatabases)
	{
		Data::ArrayListStringNN names;
		names.Add(Text::String::New(CSTR("Database")));
		Data::ArrayListObj<Optional<Text::String>> values;
		values.Add(this->dbName->Clone());
		NN<DB::SQL::SQLStringReader> reader;
		NEW_CLASSNN(reader, DB::SQL::SQLStringReader(names, values));
		return reader;
	}
	cmd.Delete();
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Unsupported SQL: "));
	sb.Append(sql);
	OPTSTR_DEL(this->lastErrorMsg);
	this->lastErrorMsg = Text::String::New(sb.ToCString());
	this->lastDataError = DB::DBConn::DataError::ExecSQLError;
#if defined(VERBOSE)
	printf("Unsupported SQL: %s\r\n", sql.v.Ptr());
#endif
	return nullptr;
}

Bool DB::SQL::SQLEngine::IsLastDataError()
{
	return this->lastDataError == DB::DBConn::DataError::ExecSQLError;
}

Optional<DB::DBTransaction> DB::SQL::SQLEngine::BeginTransaction()
{
	return nullptr;
}

void DB::SQL::SQLEngine::Commit(NN<DB::DBTransaction> tran)
{
}

void DB::SQL::SQLEngine::Rollback(NN<DB::DBTransaction> tran)
{
}

void DB::SQL::SQLEngine::AddDatabase(NN<DB::SharedReadingDB> db, Text::CString dbSchema, Text::CString sqlSchema)
{
	Sync::MutexUsage mutUsage;
	NN<DB::ReadingDB> rdb = db->UseDB(mutUsage);
	Data::ArrayListStringNN tableNames;
	if (rdb->QueryTableNames(dbSchema, tableNames) == 0)
	{
		return;
	}
	NN<Data::FastStringMapNN<SQLEngineTable>> tableMap;
	if (!this->tables.GetC(sqlSchema.OrEmpty()).SetTo(tableMap))
	{
		NEW_CLASSNN(tableMap, Data::FastStringMapNN<SQLEngineTable>());
		this->tables.PutC(sqlSchema.OrEmpty(), tableMap);
	}

	NN<SQLEngineTable> table;
	UIntOS i = 0;
	UIntOS j = tableNames.GetCount();
	while (i < j)
	{
		NN<Text::String> tableName = tableNames.GetItemNoCheck(i);
		NEW_CLASSNN(table, SQLEngineDBTable(db, dbSchema, tableName->ToCString()));
		if (tableMap->PutNN(tableName, table).SetTo(table))
		{
			table.Delete();
		}
		i++;
	}
	tableNames.FreeAll();
}
