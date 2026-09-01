#ifndef _SM_DB_SQLFILEDB
#define _SM_DB_SQLFILEDB
#include "DB/ReadingDB.h"
#include "DB/SQL/SQLCreateTableCommand.h"
#include "DB/SQL/SQLFile.h"
#include "DB/SQL/SQLInsertCommand.h"

namespace DB
{
	namespace SQL
	{
		class SQLFileDB : public DB::ReadingDB
		{
		public:
			struct TableInfo
			{
				NN<SQLObjectPath> path;
				Optional<SQLCreateTableCommand> createCmd;
				Data::ArrayListNN<SQLInsertCommand> insertCmds;
			};

			struct SchemaInfo
			{
				NN<Text::String> name;
				Data::FastStringMapNN<TableInfo> tables;
			};
		private:
			NN<SQLFile> sqlFile;
			Data::FastStringMapNN<SchemaInfo> schemaMap;
			Int8 tzQhr;

			static void __stdcall FreeTableInfo(NN<TableInfo> tableInfo);
			static void __stdcall FreeSchemaInfo(NN<SchemaInfo> schemaInfo);
			Optional<TableInfo> GetTableInfo(Text::CString schemaName, Text::CStringNN tableName);
		public:
			SQLFileDB(NN<SQLFile> sqlFile);
			virtual ~SQLFileDB();
		
			virtual UIntOS QuerySchemaNames(NN<Data::ArrayListStringNN> names);
			virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names); //Need Release
			virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
			virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
			virtual void CloseReader(NN<DBReader> r);
			virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
			virtual void Reconnect();
			virtual Int8 GetTzQhr() const;
			virtual void ForceTzQhr(Int8 tzQhr);

			virtual UIntOS GetDatabaseNames(NN<Data::ArrayListStringNN> arr);
			virtual void ReleaseDatabaseNames(NN<Data::ArrayListStringNN> arr);
			virtual Bool ChangeDatabase(Text::CStringNN databaseName);
			virtual Optional<Text::String> GetCurrDBName();

			virtual Bool IsFullConn() const; //false = read only, true = DBConn
			virtual Bool IsDBTool() const; //true = ReadingDBTool
		};
	}
}
#endif
