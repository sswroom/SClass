#ifndef _SM_DATA_TABLEDATA
#define _SM_DATA_TABLEDATA
#include "Data/DataSet.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "Text/CString.h"

namespace Data
{
	class TableData
	{
	public:
		Bool needRelease;
		NN<DB::ReadingDB> db;
		Optional<Text::String> schemaName;
		NN<Text::String> tableName;

	public:
		TableData(NN<DB::ReadingDB> db, Bool needRelease, Text::CString schemaName, Text::CStringNN tableName);
		~TableData();

		Optional<DB::DBReader> GetTableData();
		Bool GetColumnDataStr(Text::CStringNN columnName, NN<Data::ArrayListStringNN> str);
		Optional<Data::DataSet> GetDataSet(Text::CStringNN columnName);
		void CloseReader(NN<DB::DBReader> r);
	};
}
#endif
