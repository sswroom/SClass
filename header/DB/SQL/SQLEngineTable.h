#ifndef _SM_DB_SQLENGINETABLE
#define _SM_DB_SQLENGINETABLE
#include "Data/ArrayListStringNN.h"
#include "Data/QueryConditions.h"
#include "DB/DBReader.h"

namespace DB
{
	namespace SQL
	{
		class SQLEngineTable
		{
		public:
			virtual ~SQLEngineTable() {};

			virtual Optional<TableDef> GetTableDef() = 0;
			virtual Optional<DBReader> QueryTableData(Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition) = 0;
			virtual void CloseReader(NN<DBReader> r) = 0;
		};
	}
}
#endif
