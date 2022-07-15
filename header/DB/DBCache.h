#ifndef _SM_DB_DBCACHE
#define _SM_DB_DBCACHE
#include "Data/ICaseStringMap.h"
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
			Text::String *tableName;
			TableDef *def;
			UOSInt dataCnt;
		} TableInfo;
	private:
        DB::DBTool *db;
		DB::DBModel *model;
		Sync::Mutex tableMut;
		Data::ICaseStringMap<TableInfo*> tableMap;
		UOSInt cacheCnt;

		TableInfo *GetTableInfo(Text::CString tableName);
		TableInfo *GetTableInfo(TableDef *tableDef);
	public:
        DBCache(DB::DBModel *model, DB::DBTool *db);
        ~DBCache();

		OSInt GetRowCount(Text::CString tableName); //-1 = table not found
		UOSInt QueryTableData(Data::ArrayList<DB::DBRow*> *outRows, Text::CString tableName, DB::PageRequest *page);
		DB::DBRow *GetTableItem(Text::CString tableName, Int64 pk);
		void FreeTableData(Data::ArrayList<DB::DBRow*> *rows);
		void FreeTableItem(DB::DBRow *row);

		Bool IsTableExist(Text::CString tableName);
	};
}
#endif
