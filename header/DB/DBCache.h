#ifndef _SM_DB_DBCACHE
#define _SM_DB_DBCACHE
#include "Data/ArrayListNN.h"
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
			NotNullPtr<Text::String> tableName;
			NotNullPtr<TableDef> def;
			UOSInt dataCnt;
		} TableInfo;
	private:
		NotNullPtr<DB::DBTool> db;
		NotNullPtr<DB::DBModel> model;
		Sync::Mutex tableMut;
		Data::ICaseStringMap<TableInfo*> tableMap;
		UOSInt cacheCnt;

		TableInfo *GetTableInfo(Text::CString tableName);
		TableInfo *GetTableInfo(NotNullPtr<TableDef> tableDef);
	public:
        DBCache(NotNullPtr<DB::DBModel> model, NotNullPtr<DB::DBTool> db);
        ~DBCache();

		OSInt GetRowCount(Text::CString tableName); //-1 = table not found
		UOSInt QueryTableData(NotNullPtr<Data::ArrayListNN<DB::DBRow>> outRows, Text::CString tableName, DB::PageRequest *page);
		DB::DBRow *GetTableItem(Text::CString tableName, Int64 pk);
		void FreeTableData(NotNullPtr<Data::ArrayListNN<DB::DBRow>> rows);
		void FreeTableItem(NotNullPtr<DB::DBRow> row);

		Bool IsTableExist(Text::CString tableName);
	};
}
#endif
