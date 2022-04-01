#ifndef _SM_DB_DBMODEL
#define _SM_DB_DBMODEL
#include "Data/StringMap.h"
#include "DB/DBTool.h"
#include "DB/TableDef.h"

namespace DB
{
	class DBModel
	{
	private:
		Data::ArrayList<TableDef*> *tables;
		Data::StringMap<TableDef*> *tableMap;

	public:
		DBModel();
		~DBModel();

		Bool LoadDatabase(DB::DBTool *db, Text::CString dbName);
		TableDef *GetTable(Text::CString tableName);
		UOSInt GetTableNames(Data::ArrayList<Text::CString> *tableNames);
	};
}
#endif
