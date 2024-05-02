#ifndef _SM_DB_DBMODEL
#define _SM_DB_DBMODEL
#include "Data/ICaseStringMapNN.h"
#include "DB/DBTool.h"
#include "DB/TableDef.h"

namespace DB
{
	class DBModel
	{
	private:
		Data::ArrayListNN<TableDef> tables;
		Data::ICaseStringMapNN<TableDef> tableMap;

	public:
		DBModel();
		~DBModel();

		Bool LoadDatabase(NN<DB::DBTool> db, Text::CString dbName, Text::CString schemaName);
		Optional<TableDef> GetTable(Text::CStringNN tableName);
		UOSInt GetTableNames(NN<Data::ArrayList<Text::CString>> tableNames);
	};
}
#endif
