#ifndef _SM_DB_TEXTDB
#define _SM_DB_TEXTDB
#include "Data/ArrayListArr.h"
#include "Data/StringMap.h"
#include "DB/ReadingDB.h"
#include "Text/String.h"

namespace DB
{
	class TextDB : public DB::ReadingDB
	{
	public:
		struct DBData
		{
			NN<Text::String> name;
			Data::ArrayListStringNN colList;
			Data::ArrayListArr<Optional<Text::String>> valList;
		};

	private:
		Data::FastStringMapNN<DBData> dbMap;
		Optional<DBData> currDB;
	public:
		TextDB(Text::CStringNN sourceName);
		virtual ~TextDB();

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		Bool AddTable(Text::CStringNN tableName, NN<Data::ArrayListStringNN> colList);
		Bool AddTable(Text::CStringNN tableName, UnsafeArray<Text::CStringNN> colArr, UOSInt colCount);
		Bool AddTableData(NN<Data::ArrayList<Optional<Text::String>>> valList);
		Bool AddTableData(UnsafeArray<Text::CString> valArr, UOSInt colCount);
	};
}
#endif
