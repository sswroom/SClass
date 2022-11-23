#ifndef _SM_DB_TEXTDB
#define _SM_DB_TEXTDB
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
			Text::String *name;
			Data::ArrayList<const UTF8Char *> colList;
			Data::ArrayList<Text::String **> valList;
		};

	private:
		Data::StringMap<DBData*> dbMap;
		DBData *currDB;
	public:
		TextDB(Text::CString sourceName);
		virtual ~TextDB();

		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names);
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual void Reconnect();

		Bool AddTable(Text::CString tableName, Data::ArrayList<const UTF8Char*> *colList);
		Bool AddTableData(Data::ArrayList<const UTF8Char*> *valList);
	};
}
#endif
