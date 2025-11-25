#ifndef _SM_DB_DBCACHE
#define _SM_DB_DBCACHE
#include "Data/ArrayListNN.hpp"
#include "Data/ICaseStringMapNN.hpp"
#include "DB/DBModel.h"
#include "DB/DBRow.h"
#include "DB/PageRequest.h"
#include "Sync/Mutex.h"

namespace DB
{
	class DBCache
	{
	private:
		typedef struct
		{
			NN<Text::String> tableName;
			NN<TableDef> def;
			UOSInt dataCnt;
		} TableInfo;
	private:
		NN<DB::DBTool> db;
		NN<DB::DBModel> model;
		Sync::Mutex tableMut;
		Data::ICaseStringMapNN<TableInfo> tableMap;
		UOSInt cacheCnt;

		Optional<TableInfo> GetTableInfo(Text::CStringNN tableName);
		Optional<TableInfo> GetTableInfo(NN<TableDef> tableDef);
	public:
        DBCache(NN<DB::DBModel> model, NN<DB::DBTool> db);
        ~DBCache();

		OSInt GetRowCount(Text::CStringNN tableName); //-1 = table not found
		UOSInt QueryTableData(NN<Data::ArrayListNN<DB::DBRow>> outRows, Text::CStringNN tableName, DB::PageRequest *page);
		DB::DBRow *GetTableItem(Text::CStringNN tableName, Int64 pk);
		void FreeTableData(NN<Data::ArrayListNN<DB::DBRow>> rows);
		void FreeTableItem(NN<DB::DBRow> row);

		Bool IsTableExist(Text::CStringNN tableName);
	};
}
#endif
