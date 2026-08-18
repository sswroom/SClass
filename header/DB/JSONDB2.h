#ifndef _SM_DB_JSONDB2
#define _SM_DB_JSONDB2
#include "DB/ReadingDB.h"
#include "DB/TableDef.h"
#include "Text/JSON.h"
#include "Text/JSONBuilder.h"

namespace DB
{
	class JSONDB2 : public DB::ReadingDB
	{
	private:
		Optional<Text::String> schemaName;
		Data::ArrayListStringNN headers;
		Data::ArrayListStringNN types;
		NN<Text::JSONArray> rows;
		DB::TableDef tableDef;
		Int8 tzQhr;

		Bool IsSchema(Text::CString schemaName);
	public:
		JSONDB2(Text::CString schemaName, Text::CStringNN tableName, NN<Text::JSONArray> headers, NN<Text::JSONArray> types, NN<Text::JSONArray> rows);
		virtual ~JSONDB2();

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
		virtual Int8 GetTzQhr() const;
		virtual void ForceTzQhr(Int8 tzQhr);

		static Bool BuildJSON(NN<Text::JSONBuilder> builder, NN<DB::DBReader> r, NN<DB::TableDef> tabDef);
		static Text::CStringNN ColType2Str(DB::DBUtil::ColType colType);
		static DB::DBUtil::ColType Str2ColType(Text::CStringNN colType);
	};
}
#endif
