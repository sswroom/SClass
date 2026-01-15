#ifndef _SM_DB_DBMODEL
#define _SM_DB_DBMODEL
#include "Data/ICaseStringMapNN.hpp"
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
		UIntOS GetTableNames(NN<Data::ArrayListObj<Text::CString>> tableNames);
	};
}
#endif
