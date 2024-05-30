#ifndef _SM_DB_SQLGENERATOR
#define _SM_DB_SQLGENERATOR
#include "DB/PageRequest.h"
#include "DB/SQLBuilder.h"
#include "DB/TableDef.h"

namespace DB
{
	class SQLGenerator
	{
	public:
		enum class PageStatus
		{
			Succ,
			NoOffset,
			NoPage
		};

	public:
		static void AppendColDef(DB::SQLType sqlType, NN<DB::SQLBuilder> sql, NN<DB::ColDef> col, UOSInt pkCount);
		static void AppendColType(DB::SQLType sqlType, NN<DB::SQLBuilder> sql, DB::DBUtil::ColType colType, UOSInt colSize, UOSInt colSize2, Bool autoInc, Optional<Text::String> nativeType);

		static Bool GenCreateTableCmd(NN<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName, NN<DB::TableDef> tabDef, Bool multiline);
		static Bool GenInsertCmd(NN<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName, NN<DB::DBReader> r);
		static Bool GenInsertCmd(NN<DB::SQLBuilder> sql, NN<DB::TableDef> tabDef, NN<DB::DBReader> r);
		static Bool GenInsertCmd(NN<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName, Optional<DB::TableDef> tabDef, NN<DB::DBReader> r);
		static Bool GenCreateDatabaseCmd(NN<DB::SQLBuilder> sql, Text::CStringNN databaseName, const Collation *collation);
		static Bool GenDeleteDatabaseCmd(NN<DB::SQLBuilder> sql, Text::CStringNN databaseName);
		static Bool GenCreateSchemaCmd(NN<DB::SQLBuilder> sql, Text::CStringNN schemaName);
		static Bool GenDeleteSchemaCmd(NN<DB::SQLBuilder> sql, Text::CStringNN schemaName);
		static Bool GenDropTableCmd(NN<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName);
		static Bool GenDeleteTableDataCmd(NN<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName);
		static Bool GenTruncateTableCmd(NN<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName);
		static PageStatus GenSelectCmdPage(NN<DB::SQLBuilder> sql, NN<DB::TableDef> tabDef, Optional<DB::PageRequest> page);
		static UnsafeArray<UTF8Char> GenInsertCmd(UnsafeArray<UTF8Char> sqlstr, DB::SQLType sqlType, Text::CString schemaName, Text::CStringNN tableName, NN<DB::DBReader> r);
	};
}
#endif
