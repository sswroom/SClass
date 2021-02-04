#ifndef _SM_DB_DBCACHE
#define _SM_DB_DBCACHE
#include "DB/DBModel.h"
#include "DB/DBRow.h"
#include "Sync/Mutex.h"

namespace DB
{
	class DBCache
	{
	private:
		typedef struct
		{
			const UTF8Char *tableName;
			TableDef *def;
			UOSInt dataCnt;
		} TableInfo;
	private:
        DB::DBTool *db;
		DB::DBModel *model;
		Sync::Mutex *tableMut;
		Data::StringUTF8Map<TableInfo*> *tableMap;
		UOSInt cacheCnt;

		TableInfo *GetTableInfo(const UTF8Char *tableName);
		TableInfo *GetTableInfo(TableDef *tableDef);
	public:
        DBCache(DB::DBModel *model, DB::DBTool *db);
        ~DBCache();

		OSInt GetRowCount(const UTF8Char *tableName); //-1 = table not found
		UOSInt GetTableData(Data::ArrayList<DB::DBRow*> *outRows, const UTF8Char *tableName);
		DB::DBRow *GetTableItem(const UTF8Char *tableName, Int64 pk);
		void FreeTableData(Data::ArrayList<DB::DBRow*> *rows);
		void FreeTableItem(DB::DBRow *row);
	};
}
#endif
