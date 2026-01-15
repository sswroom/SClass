#ifndef _SM_DB_JSONDB
#define _SM_DB_JSONDB
#include "DB/ReadingDB.h"
#include "Text/JSON.h"

namespace DB
{
	class JSONDB : public DB::ReadingDB
	{
	private:
		NN<Text::String> layerName;
		NN<Text::JSONArray> data;
	public:
		JSONDB(NN<Text::String> sourceName, Text::CStringNN layerName, NN<Text::JSONArray> data);
		virtual ~JSONDB();

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
	};
}
#endif
