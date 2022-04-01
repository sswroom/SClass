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
		typedef struct
		{
			Text::String *name;
			Data::ArrayList<const UTF8Char *> *colList;
			Data::ArrayList<Text::String **> *valList;
		} DBData;

	private:
		Data::StringMap<DBData*> *dbMap;
		DBData *currDB;
	public:
		TextDB(Text::CString sourceName);
		virtual ~TextDB();

		virtual UOSInt GetTableNames(Data::ArrayList<Text::CString> *names); // no need to release
		virtual DBReader *QueryTableData(Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual void CloseReader(DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual void Reconnect();

		Bool AddTable(Text::CString tableName, Data::ArrayList<const UTF8Char*> *colList);
		Bool AddTableData(Data::ArrayList<const UTF8Char*> *valList);
	};
}
#endif
