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
		static void AppendColDef(DB::SQLType sqlType, NotNullPtr<DB::SQLBuilder> sql, NotNullPtr<DB::ColDef> col, UOSInt pkCount);
		static void AppendColType(DB::SQLType sqlType, NotNullPtr<DB::SQLBuilder> sql, DB::DBUtil::ColType colType, UOSInt colSize, UOSInt colSize2, Bool autoInc, Optional<Text::String> nativeType);

		static Bool GenCreateTableCmd(NotNullPtr<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName, NotNullPtr<DB::TableDef> tabDef, Bool multiline);
		static Bool GenInsertCmd(NotNullPtr<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName, NotNullPtr<DB::DBReader> r);
		static Bool GenInsertCmd(NotNullPtr<DB::SQLBuilder> sql, NotNullPtr<DB::TableDef> tabDef, NotNullPtr<DB::DBReader> r);
		static Bool GenInsertCmd(NotNullPtr<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName, Optional<DB::TableDef> tabDef, NotNullPtr<DB::DBReader> r);
		static Bool GenCreateDatabaseCmd(NotNullPtr<DB::SQLBuilder> sql, Text::CString databaseName, const Collation *collation);
		static Bool GenDeleteDatabaseCmd(NotNullPtr<DB::SQLBuilder> sql, Text::CString databaseName);
		static Bool GenCreateSchemaCmd(NotNullPtr<DB::SQLBuilder> sql, Text::CString schemaName);
		static Bool GenDeleteSchemaCmd(NotNullPtr<DB::SQLBuilder> sql, Text::CString schemaName);
		static Bool GenDropTableCmd(NotNullPtr<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName);
		static Bool GenDeleteTableDataCmd(NotNullPtr<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName);
		static Bool GenTruncateTableCmd(NotNullPtr<DB::SQLBuilder> sql, Text::CString schemaName, Text::CStringNN tableName);
		static PageStatus GenSelectCmdPage(NotNullPtr<DB::SQLBuilder> sql, NotNullPtr<DB::TableDef> tabDef, Optional<DB::PageRequest> page);
		static UTF8Char *GenInsertCmd(UTF8Char *sqlstr, DB::SQLType sqlType, Text::CString schemaName, Text::CStringNN tableName, NotNullPtr<DB::DBReader> r);
	};
}
#endif
