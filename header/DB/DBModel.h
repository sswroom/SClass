#ifndef _SM_DB_DBMODEL
#define _SM_DB_DBMODEL
#include "Data/StringUTF8Map.h"
#include "DB/DBTool.h"
#include "DB/TableDef.h"

namespace DB
{
	class DBModel
	{
	private:
		Data::ArrayList<TableDef*> *tables;
		Data::StringUTF8Map<TableDef*> *tableMap;

	public:
		DBModel();
		~DBModel();

		Bool LoadDatabase(DB::DBTool *db, const UTF8Char *dbName);
		TableDef *GetTable(const UTF8Char *tableName);
		UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *tableNames);
	};
}
#endif
