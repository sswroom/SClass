#ifndef _SM_DB_TEXTDB
#define _SM_DB_TEXTDB
#include "Data/ArrayListNN.h"
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
			NotNullPtr<Text::String> name;
			Data::ArrayListNN<const UTF8Char> colList;
			Data::ArrayList<Text::String **> valList;
		};

	private:
		Data::StringMap<DBData*> dbMap;
		DBData *currDB;
	public:
		TextDB(Text::CStringNN sourceName);
		virtual ~TextDB();

		virtual UOSInt QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListStringNN> names);
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(NotNullPtr<DBReader> r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		Bool AddTable(Text::CString tableName, Data::ArrayList<const UTF8Char*> *colList);
		Bool AddTableData(Data::ArrayList<const UTF8Char*> *valList);
	};
}
#endif
