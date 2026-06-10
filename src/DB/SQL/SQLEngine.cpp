#include "Stdafx.h"
#include "DB/SQL/SQLCommand.h"
#include "DB/SQL/SQLEmbeddedReader.h"
#include "DB/SQL/SQLEngine.h"
#include "DB/SQL/SQLEngineActionReader.h"
#include "DB/SQL/SQLEngineDBTable.h"
#include "DB/SQL/SQLStringReader.h"
#include "DB/SQL/UseCommand.h"
#include "IO/Path.h"

#define VERBOSE

Optional<DB::DBReader> DB::SQL::SQLEngine::EmptyStringReader(NN<Data::ArrayListStringNN> returnColNames, Optional<Data::ArrayListStringNN> reqColNames)
{
	NN<Data::ArrayListStringNN> nncolNames;
	if (reqColNames.SetTo(nncolNames))
	{
		Data::FastStringMapNative<Bool> colMap;
		UIntOS i = 0;
		UIntOS j = returnColNames->GetCount();
		while (i < j)
		{
			colMap.PutNN(returnColNames->GetItemNoCheck(i), true);
			i++;
		}
		i = 0;
		j = nncolNames->GetCount();
		while (i < j)
		{
			if (!colMap.Get(nncolNames->GetItemNoCheck(i)))
			{
				Text::StringBuilderUTF8 sb;
				returnColNames->FreeAll();
				sb.Append(CSTR("SQL Error [1054] [42S22]: Unknown column '"));
				sb.Append(nncolNames->GetItemNoCheck(i));
				sb.Append(CSTR("' in 'field list'"));
				OPTSTR_DEL(this->lastErrorMsg);
				this->lastErrorMsg = Text::String::New(sb.ToCString());
				this->lastDataError = DB::DBConn::DataError::ExecSQLError;
				returnColNames->FreeAll();
				return nullptr;
			}
			i++;
		}
		returnColNames->FreeAll();

		i = 0;
		while (i < j)
		{
			returnColNames->Add(nncolNames->GetItemNoCheck(i)->Clone());
			i++;
		}
		Data::ArrayListObj<Optional<Text::String>> vals;
		NN<SQLEngineReader> reader;
		NEW_CLASSNN(reader, SQLStringReader(nncolNames, vals));
		return reader;
	}
	else
	{
		Data::ArrayListObj<Optional<Text::String>> vals;
		NN<SQLEngineReader> reader;
		NEW_CLASSNN(reader, SQLStringReader(returnColNames, vals));
		return reader;
	}
}

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
	this->currDB = this->dbName->Clone();
}

DB::SQL::SQLEngine::~SQLEngine()
{
	NN<Data::FastStringMapNN<SQLEngineTable>> table;
	UIntOS i = this->tables.GetCount();
	while (i-- > 0)
	{
		table = this->tables.GetItemNoCheck(i);
		table->DeleteAll();
		table.Delete();
	}
	OPTSTR_DEL(this->lastErrorMsg);
	this->dbName->Release();
	this->currDB->Release();
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

UIntOS DB::SQL::SQLEngine::QueryTableNamesCurrDB(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
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

UIntOS DB::SQL::SQLEngine::QueryTableNamesMySQL(Text::CStringNN dbName, Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (this->dbName->Equals(dbName))
	{
		return this->QueryTableNamesCurrDB(schemaName, names);
	}
	else if (this->sqlType == DB::SQLType::MySQL && this->currDB->Equals(CSTR("sys")))
	{
		UIntOS initCnt = names->GetCount();
		names->Add(Text::String::New(CSTR("host_summary")));
		names->Add(Text::String::New(CSTR("host_summary_by_file_io")));
		names->Add(Text::String::New(CSTR("host_summary_by_file_io_type")));
		names->Add(Text::String::New(CSTR("host_summary_by_stages")));
		names->Add(Text::String::New(CSTR("host_summary_by_statement_latency")));
		names->Add(Text::String::New(CSTR("host_summary_by_statement_type")));
		names->Add(Text::String::New(CSTR("innodb_buffer_stats_by_schema")));
		names->Add(Text::String::New(CSTR("innodb_buffer_stats_by_table")));
		names->Add(Text::String::New(CSTR("innodb_lock_waits")));
		names->Add(Text::String::New(CSTR("io_by_thread_by_latency")));
		names->Add(Text::String::New(CSTR("io_global_by_file_by_bytes")));
		names->Add(Text::String::New(CSTR("io_global_by_file_by_latency")));
		names->Add(Text::String::New(CSTR("io_global_by_wait_by_bytes")));
		names->Add(Text::String::New(CSTR("io_global_by_wait_by_latency")));
		names->Add(Text::String::New(CSTR("latest_file_io")));
		names->Add(Text::String::New(CSTR("memory_by_host_by_current_bytes")));
		names->Add(Text::String::New(CSTR("memory_by_thread_by_current_bytes")));
		names->Add(Text::String::New(CSTR("memory_by_user_by_current_bytes")));
		names->Add(Text::String::New(CSTR("memory_global_by_current_bytes")));
		names->Add(Text::String::New(CSTR("memory_global_total")));
		names->Add(Text::String::New(CSTR("metrics")));
		names->Add(Text::String::New(CSTR("processlist")));
		names->Add(Text::String::New(CSTR("ps_check_lost_instrumentation")));
		names->Add(Text::String::New(CSTR("schema_auto_increment_columns")));
		names->Add(Text::String::New(CSTR("schema_index_statistics")));
		names->Add(Text::String::New(CSTR("schema_object_overview")));
		names->Add(Text::String::New(CSTR("schema_redundant_indexes")));
		names->Add(Text::String::New(CSTR("schema_table_lock_waits")));
		names->Add(Text::String::New(CSTR("schema_table_statistics")));
		names->Add(Text::String::New(CSTR("schema_table_statistics_with_buffer")));
		names->Add(Text::String::New(CSTR("schema_tables_with_full_table_scans")));
		names->Add(Text::String::New(CSTR("schema_unused_indexes")));
		names->Add(Text::String::New(CSTR("session")));
		names->Add(Text::String::New(CSTR("session_ssl_status")));
		names->Add(Text::String::New(CSTR("statement_analysis")));
		names->Add(Text::String::New(CSTR("statements_with_errors_or_warnings")));
		names->Add(Text::String::New(CSTR("statements_with_full_table_scans")));
		names->Add(Text::String::New(CSTR("statements_with_runtimes_in_95th_percentile")));
		names->Add(Text::String::New(CSTR("statements_with_sorting")));
		names->Add(Text::String::New(CSTR("statements_with_temp_tables")));
		names->Add(Text::String::New(CSTR("sys_config")));
		names->Add(Text::String::New(CSTR("user_summary")));
		names->Add(Text::String::New(CSTR("user_summary_by_file_io")));
		names->Add(Text::String::New(CSTR("user_summary_by_file_io_type")));
		names->Add(Text::String::New(CSTR("user_summary_by_stages")));
		names->Add(Text::String::New(CSTR("user_summary_by_statement_latency")));
		names->Add(Text::String::New(CSTR("user_summary_by_statement_type")));
		names->Add(Text::String::New(CSTR("version")));
		names->Add(Text::String::New(CSTR("wait_classes_global_by_avg_latency")));
		names->Add(Text::String::New(CSTR("wait_classes_global_by_latency")));
		names->Add(Text::String::New(CSTR("waits_by_host_by_latency")));
		names->Add(Text::String::New(CSTR("waits_by_user_by_latency")));
		names->Add(Text::String::New(CSTR("waits_global_by_latency")));
		names->Add(Text::String::New(CSTR("x$host_summary")));
		names->Add(Text::String::New(CSTR("x$host_summary_by_file_io")));
		names->Add(Text::String::New(CSTR("x$host_summary_by_file_io_type")));
		names->Add(Text::String::New(CSTR("x$host_summary_by_stages")));
		names->Add(Text::String::New(CSTR("x$host_summary_by_statement_latency")));
		names->Add(Text::String::New(CSTR("x$host_summary_by_statement_type")));
		names->Add(Text::String::New(CSTR("x$innodb_buffer_stats_by_schema")));
		names->Add(Text::String::New(CSTR("x$innodb_buffer_stats_by_table")));
		names->Add(Text::String::New(CSTR("x$innodb_lock_waits")));
		names->Add(Text::String::New(CSTR("x$io_by_thread_by_latency")));
		names->Add(Text::String::New(CSTR("x$io_global_by_file_by_bytes")));
		names->Add(Text::String::New(CSTR("x$io_global_by_file_by_latency")));
		names->Add(Text::String::New(CSTR("x$io_global_by_wait_by_bytes")));
		names->Add(Text::String::New(CSTR("x$io_global_by_wait_by_latency")));
		names->Add(Text::String::New(CSTR("x$latest_file_io")));
		names->Add(Text::String::New(CSTR("x$memory_by_host_by_current_bytes")));
		names->Add(Text::String::New(CSTR("x$memory_by_thread_by_current_bytes")));
		names->Add(Text::String::New(CSTR("x$memory_by_user_by_current_bytes")));
		names->Add(Text::String::New(CSTR("x$memory_global_by_current_bytes")));
		names->Add(Text::String::New(CSTR("x$memory_global_total")));
		names->Add(Text::String::New(CSTR("x$processlist")));
		names->Add(Text::String::New(CSTR("x$ps_digest_95th_percentile_by_avg_us")));
		names->Add(Text::String::New(CSTR("x$ps_digest_avg_latency_distribution")));
		names->Add(Text::String::New(CSTR("x$ps_schema_table_statistics_io")));
		names->Add(Text::String::New(CSTR("x$schema_flattened_keys")));
		names->Add(Text::String::New(CSTR("x$schema_index_statistics")));
		names->Add(Text::String::New(CSTR("x$schema_table_lock_waits")));
		names->Add(Text::String::New(CSTR("x$schema_table_statistics")));
		names->Add(Text::String::New(CSTR("x$schema_table_statistics_with_buffer")));
		names->Add(Text::String::New(CSTR("x$schema_tables_with_full_table_scans")));
		names->Add(Text::String::New(CSTR("x$session")));
		names->Add(Text::String::New(CSTR("x$statement_analysis")));
		names->Add(Text::String::New(CSTR("x$statements_with_errors_or_warnings")));
		names->Add(Text::String::New(CSTR("x$statements_with_full_table_scans")));
		names->Add(Text::String::New(CSTR("x$statements_with_runtimes_in_95th_percentile")));
		names->Add(Text::String::New(CSTR("x$statements_with_sorting")));
		names->Add(Text::String::New(CSTR("x$statements_with_temp_tables")));
		names->Add(Text::String::New(CSTR("x$user_summary")));
		names->Add(Text::String::New(CSTR("x$user_summary_by_file_io")));
		names->Add(Text::String::New(CSTR("x$user_summary_by_file_io_type")));
		names->Add(Text::String::New(CSTR("x$user_summary_by_stages")));
		names->Add(Text::String::New(CSTR("x$user_summary_by_statement_latency")));
		names->Add(Text::String::New(CSTR("x$user_summary_by_statement_type")));
		names->Add(Text::String::New(CSTR("x$wait_classes_global_by_avg_latency")));
		names->Add(Text::String::New(CSTR("x$wait_classes_global_by_latency")));
		names->Add(Text::String::New(CSTR("x$waits_by_host_by_latency")));
		names->Add(Text::String::New(CSTR("x$waits_by_user_by_latencyity")));
		names->Add(Text::String::New(CSTR("x$waits_global_by_latency")));
		return names->GetCount() - initCnt;
	}
	else if (this->sqlType == DB::SQLType::MySQL && this->currDB->Equals(CSTR("information_schema")))
	{
		UIntOS initCnt = names->GetCount();
		names->Add(Text::String::New(CSTR("ADMINISTRABLE_ROLE_AUTHORIZATIONS")));
		names->Add(Text::String::New(CSTR("APPLICABLE_ROLES")));
		names->Add(Text::String::New(CSTR("CHARACTER_SETS")));
		names->Add(Text::String::New(CSTR("CHECK_CONSTRAINTS")));
		names->Add(Text::String::New(CSTR("COLLATIONS")));
		names->Add(Text::String::New(CSTR("COLLATION_CHARACTER_SET_APPLICABILITY")));
		names->Add(Text::String::New(CSTR("COLUMNS")));
		names->Add(Text::String::New(CSTR("COLUMNS_EXTENSIONS")));
		names->Add(Text::String::New(CSTR("COLUMN_PRIVILEGES")));
		names->Add(Text::String::New(CSTR("COLUMN_STATISTICS")));
		names->Add(Text::String::New(CSTR("ENABLED_ROLES")));
		names->Add(Text::String::New(CSTR("ENGINES")));
		names->Add(Text::String::New(CSTR("EVENTS")));
		names->Add(Text::String::New(CSTR("FILES")));
		names->Add(Text::String::New(CSTR("INNODB_BUFFER_PAGE")));
		names->Add(Text::String::New(CSTR("INNODB_BUFFER_PAGE_LRU")));
		names->Add(Text::String::New(CSTR("INNODB_BUFFER_POOL_STATS")));
		names->Add(Text::String::New(CSTR("INNODB_CACHED_INDEXES")));
		names->Add(Text::String::New(CSTR("INNODB_CMP")));
		names->Add(Text::String::New(CSTR("INNODB_CMPMEM")));
		names->Add(Text::String::New(CSTR("INNODB_CMPMEM_RESET")));
		names->Add(Text::String::New(CSTR("INNODB_CMP_PER_INDEX")));
		names->Add(Text::String::New(CSTR("INNODB_CMP_PER_INDEX_RESET")));
		names->Add(Text::String::New(CSTR("INNODB_CMP_RESET")));
		names->Add(Text::String::New(CSTR("INNODB_COLUMNS")));
		names->Add(Text::String::New(CSTR("INNODB_DATAFILES")));
		names->Add(Text::String::New(CSTR("INNODB_FIELDS")));
		names->Add(Text::String::New(CSTR("INNODB_FOREIGN")));
		names->Add(Text::String::New(CSTR("INNODB_FOREIGN_COLS")));
		names->Add(Text::String::New(CSTR("INNODB_FT_BEING_DELETED")));
		names->Add(Text::String::New(CSTR("INNODB_FT_CONFIG")));
		names->Add(Text::String::New(CSTR("INNODB_FT_DEFAULT_STOPWORD")));
		names->Add(Text::String::New(CSTR("INNODB_FT_DELETED")));
		names->Add(Text::String::New(CSTR("INNODB_FT_INDEX_CACHE")));
		names->Add(Text::String::New(CSTR("INNODB_FT_INDEX_TABLE")));
		names->Add(Text::String::New(CSTR("INNODB_INDEXES")));
		names->Add(Text::String::New(CSTR("INNODB_METRICS")));
		names->Add(Text::String::New(CSTR("INNODB_SESSION_TEMP_TABLESPACES")));
		names->Add(Text::String::New(CSTR("INNODB_TABLES")));
		names->Add(Text::String::New(CSTR("INNODB_TABLESPACES")));
		names->Add(Text::String::New(CSTR("INNODB_TABLESPACES_BRIEF")));
		names->Add(Text::String::New(CSTR("INNODB_TABLESTATS")));
		names->Add(Text::String::New(CSTR("INNODB_TEMP_TABLE_INFO")));
		names->Add(Text::String::New(CSTR("INNODB_TRX")));
		names->Add(Text::String::New(CSTR("INNODB_VIRTUAL")));
		names->Add(Text::String::New(CSTR("KEYWORDS")));
		names->Add(Text::String::New(CSTR("KEY_COLUMN_USAGE")));
		names->Add(Text::String::New(CSTR("OPTIMIZER_TRACE")));
		names->Add(Text::String::New(CSTR("PARAMETERS")));
		names->Add(Text::String::New(CSTR("PARTITIONS")));
		names->Add(Text::String::New(CSTR("PLUGINS")));
		names->Add(Text::String::New(CSTR("PROCESSLIST")));
		names->Add(Text::String::New(CSTR("PROFILING")));
		names->Add(Text::String::New(CSTR("REFERENTIAL_CONSTRAINTS")));
		names->Add(Text::String::New(CSTR("RESOURCE_GROUPS")));
		names->Add(Text::String::New(CSTR("ROLE_COLUMN_GRANTS")));
		names->Add(Text::String::New(CSTR("ROLE_ROUTINE_GRANTS")));
		names->Add(Text::String::New(CSTR("ROLE_TABLE_GRANTS")));
		names->Add(Text::String::New(CSTR("ROUTINES")));
		names->Add(Text::String::New(CSTR("SCHEMATA")));
		names->Add(Text::String::New(CSTR("SCHEMATA_EXTENSIONS")));
		names->Add(Text::String::New(CSTR("SCHEMA_PRIVILEGES")));
		names->Add(Text::String::New(CSTR("STATISTICS")));
		names->Add(Text::String::New(CSTR("ST_GEOMETRY_COLUMNS")));
		names->Add(Text::String::New(CSTR("ST_SPATIAL_REFERENCE_SYSTEMS")));
		names->Add(Text::String::New(CSTR("ST_UNITS_OF_MEASURE")));
		names->Add(Text::String::New(CSTR("TABLES")));
		names->Add(Text::String::New(CSTR("TABLESPACES_EXTENSIONS")));
		names->Add(Text::String::New(CSTR("TABLES_EXTENSIONS")));
		names->Add(Text::String::New(CSTR("TABLE_CONSTRAINTS")));
		names->Add(Text::String::New(CSTR("TABLE_CONSTRAINTS_EXTENSIONS")));
		names->Add(Text::String::New(CSTR("TABLE_PRIVILEGES")));
		names->Add(Text::String::New(CSTR("TRIGGERS")));
		names->Add(Text::String::New(CSTR("USER_ATTRIBUTES")));
		names->Add(Text::String::New(CSTR("USER_PRIVILEGES")));
		names->Add(Text::String::New(CSTR("VIEWS")));
		names->Add(Text::String::New(CSTR("VIEW_ROUTINE_USAGE")));
		names->Add(Text::String::New(CSTR("VIEW_TABLE_USAGE")));
		return names->GetCount() - initCnt;
	}
	return 0;
}

UIntOS DB::SQL::SQLEngine::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (this->sqlType == DB::SQLType::MySQL)
	{
		return this->QueryTableNamesMySQL(this->currDB->ToCString(), schemaName, names);
	}
	else if (this->currDB->Equals(this->dbName))
	{
		return this->QueryTableNamesCurrDB(schemaName, names);
	}
	return 0;
}

Optional<DB::DBReader> DB::SQL::SQLEngine::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<SQLEngineTable> table;
	NN<Data::FastStringMapNN<SQLEngineTable>> tableMap;
	if (this->currDB->Equals(this->dbName))
	{
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
	}
	else if (this->sqlType == DB::SQLType::MySQL && this->currDB->Equals(CSTR("sys")))
	{
		if (tableName.Equals(CSTR("host_summary")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("host")));
			tableCols.Add(Text::String::New(CSTR("statements")));
			tableCols.Add(Text::String::New(CSTR("statement_latency")));
			tableCols.Add(Text::String::New(CSTR("statement_avg_latency")));
			tableCols.Add(Text::String::New(CSTR("table_scans")));
			tableCols.Add(Text::String::New(CSTR("file_ios")));
			tableCols.Add(Text::String::New(CSTR("file_io_latency")));
			tableCols.Add(Text::String::New(CSTR("current_connections")));
			tableCols.Add(Text::String::New(CSTR("total_connections")));
			tableCols.Add(Text::String::New(CSTR("unique_users")));
			tableCols.Add(Text::String::New(CSTR("current_memory")));
			tableCols.Add(Text::String::New(CSTR("total_memory_allocated")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("host_summary_by_file_io")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("host")));
			tableCols.Add(Text::String::New(CSTR("ios")));
			tableCols.Add(Text::String::New(CSTR("io_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("host_summary_by_file_io_type")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("host")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("host_summary_by_stages")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("host")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("avg_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("host_summary_by_statement_latency")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("host")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			tableCols.Add(Text::String::New(CSTR("lock_latency")));
			tableCols.Add(Text::String::New(CSTR("cpu_latency")));
			tableCols.Add(Text::String::New(CSTR("rows_sent")));
			tableCols.Add(Text::String::New(CSTR("rows_examined")));
			tableCols.Add(Text::String::New(CSTR("rows_affected")));
			tableCols.Add(Text::String::New(CSTR("full_scans")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		/////////////////////////////
		else if (tableName.Equals(CSTR("host_summary_by_statement_type")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("host")));
			tableCols.Add(Text::String::New(CSTR("statement_type")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("innodb_buffer_stats_by_schema")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("schema_name")));
			tableCols.Add(Text::String::New(CSTR("pool_size")));
			tableCols.Add(Text::String::New(CSTR("free_buffers")));
			tableCols.Add(Text::String::New(CSTR("database_pages")));
			tableCols.Add(Text::String::New(CSTR("modified_pages")));
			tableCols.Add(Text::String::New(CSTR("pages_read")));
			tableCols.Add(Text::String::New(CSTR("pages_written")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("innodb_buffer_stats_by_table")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("schema_name")));
			tableCols.Add(Text::String::New(CSTR("table_name")));
			tableCols.Add(Text::String::New(CSTR("pool_size")));
			tableCols.Add(Text::String::New(CSTR("free_buffers")));
			tableCols.Add(Text::String::New(CSTR("database_pages")));
			tableCols.Add(Text::String::New(CSTR("modified_pages")));
			tableCols.Add(Text::String::New(CSTR("pages_read")));
			tableCols.Add(Text::String::New(CSTR("pages_written")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("innodb_lock_waits")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("requesting_trx_id")));
			tableCols.Add(Text::String::New(CSTR("requested_lock_id")));
			tableCols.Add(Text::String::New(CSTR("blocking_trx_id")));
			tableCols.Add(Text::String::New(CSTR("blocking_lock_id")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("io_by_thread_by_latency")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("thread_id")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("io_global_by_file_by_bytes")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("io_global_by_file_by_latency")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("io_global_by_wait_by_bytes")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("io_global_by_wait_by_latency")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("latest_file_io")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("thread_id")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("file_name")));
			tableCols.Add(Text::String::New(CSTR("latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("memory_by_host_by_current_bytes")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("host")));
			tableCols.Add(Text::String::New(CSTR("current_bytes")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("memory_by_thread_by_current_bytes")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("thread_id")));
			tableCols.Add(Text::String::New(CSTR("current_bytes")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("memory_by_user_by_current_bytes")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("user")));
			tableCols.Add(Text::String::New(CSTR("current_bytes")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("memory_global_by_current_bytes")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("current_bytes")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("memory_global_total")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("total_bytes")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("metrics")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("name")));
			tableCols.Add(Text::String::New(CSTR("subsystem")));
			tableCols.Add(Text::String::New(CSTR("count")));
			tableCols.Add(Text::String::New(CSTR("avg_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("processlist")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("id")));
			tableCols.Add(Text::String::New(CSTR("user")));
			tableCols.Add(Text::String::New(CSTR("host")));
			tableCols.Add(Text::String::New(CSTR("db")));
			tableCols.Add(Text::String::New(CSTR("command")));
			tableCols.Add(Text::String::New(CSTR("time")));
			tableCols.Add(Text::String::New(CSTR("state")));
			tableCols.Add(Text::String::New(CSTR("info")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("ps_check_lost_instrumentation")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("object_schema")));
			tableCols.Add(Text::String::New(CSTR("object_name")));
			tableCols.Add(Text::String::New(CSTR("object_type")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("schema_auto_increment_columns")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("table_schema")));
			tableCols.Add(Text::String::New(CSTR("table_name")));
			tableCols.Add(Text::String::New(CSTR("column_name")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("schema_index_statistics")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("table_schema")));
			tableCols.Add(Text::String::New(CSTR("table_name")));
			tableCols.Add(Text::String::New(CSTR("index_name")));
			tableCols.Add(Text::String::New(CSTR("last_update")));
			tableCols.Add(Text::String::New(CSTR("stat_name")));
			tableCols.Add(Text::String::New(CSTR("stat_value")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("schema_object_overview")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("object_type")));
			tableCols.Add(Text::String::New(CSTR("object_schema")));
			tableCols.Add(Text::String::New(CSTR("object_name")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("schema_redundant_indexes")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("table_schema")));
			tableCols.Add(Text::String::New(CSTR("table_name")));
			tableCols.Add(Text::String::New(CSTR("index_name")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("schema_table_lock_waits")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("object_schema")));
			tableCols.Add(Text::String::New(CSTR("object_name")));
			tableCols.Add(Text::String::New(CSTR("object_type")));
			tableCols.Add(Text::String::New(CSTR("lock_type")));
			tableCols.Add(Text::String::New(CSTR("lock_duration")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("schema_table_statistics")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("table_schema")));
			tableCols.Add(Text::String::New(CSTR("table_name")));
			tableCols.Add(Text::String::New(CSTR("rows_fetched")));
			tableCols.Add(Text::String::New(CSTR("rows_inserted")));
			tableCols.Add(Text::String::New(CSTR("rows_updated")));
			tableCols.Add(Text::String::New(CSTR("rows_deleted")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("schema_table_statistics_with_buffer")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("table_schema")));
			tableCols.Add(Text::String::New(CSTR("table_name")));
			tableCols.Add(Text::String::New(CSTR("rows_fetched")));
			tableCols.Add(Text::String::New(CSTR("rows_inserted")));
			tableCols.Add(Text::String::New(CSTR("rows_updated")));
			tableCols.Add(Text::String::New(CSTR("rows_deleted")));
			tableCols.Add(Text::String::New(CSTR("fetch_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("schema_tables_with_full_table_scans")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("table_schema")));
			tableCols.Add(Text::String::New(CSTR("table_name")));
			tableCols.Add(Text::String::New(CSTR("full_table_scans")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("schema_unused_indexes")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("table_schema")));
			tableCols.Add(Text::String::New(CSTR("table_name")));
			tableCols.Add(Text::String::New(CSTR("index_name")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("session")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("thread_id")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("state")));
			tableCols.Add(Text::String::New(CSTR("duration")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("session_ssl_status")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("thread_id")));
			tableCols.Add(Text::String::New(CSTR("ssl_cipher")));
			tableCols.Add(Text::String::New(CSTR("ssl_version")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		 else if (tableName.Equals(CSTR("session_status")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("variable_name")));
			tableCols.Add(Text::String::New(CSTR("variable_value")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("session_variables")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("variable_name")));
			tableCols.Add(Text::String::New(CSTR("variable_value")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		 else if (tableName.Equals(CSTR("statement_analysis")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("digest")));
			tableCols.Add(Text::String::New(CSTR("sample_user")));
			tableCols.Add(Text::String::New(CSTR("sample_host")));
			tableCols.Add(Text::String::New(CSTR("sample_db")));
			tableCols.Add(Text::String::New(CSTR("avg_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("statements_with_errors_or_warnings")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("digest")));
			tableCols.Add(Text::String::New(CSTR("error_count")));
			tableCols.Add(Text::String::New(CSTR("warning_count")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("statements_with_full_table_scans")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("digest")));
			tableCols.Add(Text::String::New(CSTR("full_table_scans")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("statements_with_runtimes_in_95th_percentile")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("digest")));
			tableCols.Add(Text::String::New(CSTR("avg_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("statements_with_sorting")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("digest")));
			tableCols.Add(Text::String::New(CSTR("sorts")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("statements_with_temp_tables")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("digest")));
			tableCols.Add(Text::String::New(CSTR("temp_tables")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("sys_config")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("variable_name")));
			tableCols.Add(Text::String::New(CSTR("variable_value")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("user_summary")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("user")));
			tableCols.Add(Text::String::New(CSTR("statements")));
			tableCols.Add(Text::String::New(CSTR("statement_latency")));
			tableCols.Add(Text::String::New(CSTR("statement_avg_latency")));
			tableCols.Add(Text::String::New(CSTR("table_scans")));
			tableCols.Add(Text::String::New(CSTR("file_ios")));
			tableCols.Add(Text::String::New(CSTR("file_io_latency")));
			tableCols.Add(Text::String::New(CSTR("current_connections")));
			tableCols.Add(Text::String::New(CSTR("total_connections")));
			tableCols.Add(Text::String::New(CSTR("unique_hosts")));
			tableCols.Add(Text::String::New(CSTR("current_memory")));
			tableCols.Add(Text::String::New(CSTR("total_memory_allocated")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("user_summary_by_file_io")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("user")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("ios")));
			tableCols.Add(Text::String::New(CSTR("io_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("user_summary_by_file_io_type")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("user")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("user_summary_by_stages")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("user")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("user_summary_by_statement_latency")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("user")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("user_summary_by_statement_type")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("user")));
			tableCols.Add(Text::String::New(CSTR("statement_type")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("version")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("version")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("wait_classes_global_by_avg_latency")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("avg_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("wait_classes_global_by_latency")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("waits_by_host_by_latency")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("host")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("waits_by_user_by_latency")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("user")));
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
		else if (tableName.Equals(CSTR("waits_global_by_latency")))
		{
			Data::ArrayListStringNN tableCols;
			tableCols.Add(Text::String::New(CSTR("event_name")));
			tableCols.Add(Text::String::New(CSTR("total")));
			tableCols.Add(Text::String::New(CSTR("total_latency")));
			tableCols.Add(Text::String::New(CSTR("max_latency")));
			return this->EmptyStringReader(tableCols, colNames);
		}
	}
	else if (this->sqlType == DB::SQLType::MySQL && this->currDB->Equals(CSTR("information_schema")))
	{

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
	arr->Add(this->dbName->Clone());
	arr->Add(Text::String::New(CSTR("sys")));
	arr->Add(Text::String::New(CSTR("information_schema")));
	return 3;
}

void DB::SQL::SQLEngine::ReleaseDatabaseNames(NN<Data::ArrayListStringNN> arr)
{
	arr->FreeAll();
}

Bool DB::SQL::SQLEngine::ChangeDatabase(Text::CStringNN databaseName)
{
	if (databaseName.Equals(this->dbName))
	{
		this->currDB->Release();
		this->currDB = this->dbName->Clone();
		return true;
	}
	else if (this->sqlType == DB::SQLType::MySQL && databaseName.Equals(CSTR("sys")))
	{
		this->currDB->Release();
		this->currDB = Text::String::New(CSTR("sys"));
		return true;
	}
	else if (this->sqlType == DB::SQLType::MySQL && databaseName.Equals(CSTR("information_schema")))
	{
		this->currDB->Release();
		this->currDB = Text::String::New(CSTR("information_schema"));
		return true;
	}
	return false;
}

Optional<Text::String> DB::SQL::SQLEngine::GetCurrDBName()
{
	return this->currDB;
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
	NN<DB::DBReader> r;
	if (this->ExecuteReader(sql).SetTo(r))
	{
		IntOS rowAffected = NN<SQLEngineReader>::ConvertFrom(r)->GetRowChanged();
		this->CloseReader(r);
		return rowAffected;
	}
	return -1;
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
		Data::ArrayListObj<Optional<Text::String>> values;
		this->GetDatabaseNames(names);
		UIntOS i = 0;
		UIntOS j = names.GetCount();
		while (i < j)
		{
			values.Add(names.GetItemNoCheck(i));
			i++;
		}
		names.Clear();
		names.Add(Text::String::New(CSTR("Database")));
		NN<DB::SQL::SQLStringReader> reader;
		NEW_CLASSNN(reader, DB::SQL::SQLStringReader(names, values));
		cmd.Delete();
		return reader;
	}
	else if (cmd->GetCommandType() == DB::SQL::CommandType::Use)
	{
		NN<DB::SQL::UseCommand> useCmd = NN<DB::SQL::UseCommand>::ConvertFrom(cmd);
		if (this->ChangeDatabase(useCmd->GetDBName()->ToCString()))
		{
			cmd.Delete();
			NN<DB::SQL::SQLEngineActionReader> reader;
			NEW_CLASSNN(reader, DB::SQL::SQLEngineActionReader(0));
			return reader;
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("SQL Error [1049] [42000]: Unknown database: '"));
			sb.Append(useCmd->GetDBName());
			sb.AppendC(UTF8STRC("'"));
			OPTSTR_DEL(this->lastErrorMsg);
			this->lastErrorMsg = Text::String::New(sb.ToCString());
			this->lastDataError = DB::DBConn::DataError::ExecSQLError;
		}
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
