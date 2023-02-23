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
		static void AppendColDef(DB::DBUtil::SQLType sqlType, DB::SQLBuilder *sql, DB::ColDef *col);
		static void AppendColType(DB::DBUtil::SQLType sqlType, DB::SQLBuilder *sql, DB::DBUtil::ColType colType, UOSInt colSize, UOSInt colSize2, Bool autoInc, Text::String *nativeType);

		static Bool GenCreateTableCmd(DB::SQLBuilder *sql, Text::CString schemaName, Text::CString tableName, DB::TableDef *tabDef, Bool multiline);
		static Bool GenInsertCmd(DB::SQLBuilder *sql, Text::CString schemaName, Text::CString tableName, DB::DBReader *r);
		static Bool GenInsertCmd(DB::SQLBuilder *sql, DB::TableDef *tabDef, DB::DBReader *r);
		static Bool GenInsertCmd(DB::SQLBuilder *sql, Text::CString schemaName, Text::CString tableName, DB::TableDef *tabDef, DB::DBReader *r);
		static Bool GenCreateDatabaseCmd(DB::SQLBuilder *sql, Text::CString databaseName);
		static Bool GenDeleteDatabaseCmd(DB::SQLBuilder *sql, Text::CString databaseName);
		static Bool GenCreateSchemaCmd(DB::SQLBuilder *sql, Text::CString schemaName);
		static Bool GenDeleteSchemaCmd(DB::SQLBuilder *sql, Text::CString schemaName);
		static Bool GenDropTableCmd(DB::SQLBuilder *sql, Text::CString tableName);
		static Bool GenDeleteTableDataCmd(DB::SQLBuilder *sql, Text::CString schemaName, Text::CString tableName);
		static Bool GenTruncateTableCmd(DB::SQLBuilder *sql, Text::CString schemaName, Text::CString tableName);
		static PageStatus GenSelectCmdPage(DB::SQLBuilder *sql, DB::TableDef *tabDef, DB::PageRequest *page);
		static UTF8Char *GenInsertCmd(UTF8Char *sqlstr, DB::DBUtil::SQLType sqlType, Text::CString tableName, DB::DBReader *r);
	};
}
#endif
