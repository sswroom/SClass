#ifndef _SM_DB_SQLENGINEDBTABLE
#define _SM_DB_SQLENGINEDBTABLE
#include "DB/SharedReadingDB.h"
#include "DB/SQL/SQLEngineTable.h"

namespace DB
{
	namespace SQL
	{
		class SQLEngineDBTable : public SQLEngineTable
		{
		private:
			NN<DB::SharedReadingDB> db;
			Optional<Text::String> schemaName;
			NN<Text::String> tableName;
		public:
			SQLEngineDBTable(NN<DB::SharedReadingDB> db, Text::CString schemaName, Text::CStringNN tableName);
			virtual ~SQLEngineDBTable();

			virtual Optional<TableDef> GetTableDef();
			virtual Optional<DBReader> QueryTableData(Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
			virtual void CloseReader(NN<DBReader> r);
		};
	}
}
#endif
