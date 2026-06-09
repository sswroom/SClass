#ifndef _SM_DB_SQLENGINE
#define _SM_DB_SQLENGINE
#include "Data/ArrayListObj.hpp"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/SharedReadingDB.h"
#include "DB/SQL/SQLEngineTable.h"

namespace DB
{
	namespace SQL
	{
		class SQLEngine : public DBConn
		{
		private:
			DB::SQLType sqlType;
			Int8 tzQhr;
			Optional<Text::String> lastErrorMsg;
			Data::FastStringMapNN<Data::FastStringMapNN<SQLEngineTable>> tables;
			NN<Text::String> dbName;
			NN<Text::String> currDB;
		public:
			SQLEngine(DB::SQLType sqlType, Text::CStringNN sourceName);
			virtual ~SQLEngine();

			virtual UIntOS QuerySchemaNames(NN<Data::ArrayListStringNN> names);
			UIntOS QueryTableNamesCurrDB(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
			UIntOS QueryTableNamesMySQL(Text::CStringNN dbName, Text::CString schemaName, NN<Data::ArrayListStringNN> names);
			virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
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

			virtual DB::SQLType GetSQLType() const;
			virtual ConnType GetConnType() const;
			virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
			virtual void Close();
			virtual IntOS ExecuteNonQuery(Text::CStringNN sql);
			virtual Optional<DBReader> ExecuteReader(Text::CStringNN sql);
			virtual Bool IsLastDataError();

			virtual Optional<DBTransaction> BeginTransaction();
			virtual void Commit(NN<DBTransaction> tran);
			virtual void Rollback(NN<DBTransaction> tran);

			void AddDatabase(NN<DB::SharedReadingDB> db, Text::CString dbSchema, Text::CString sqlSchema);
		};
	}
}
#endif
