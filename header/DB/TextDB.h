#ifndef _SM_DB_TEXTDB
#define _SM_DB_TEXTDB
#include "Data/StringUTF8Map.h"
#include "DB/ReadingDB.h"

namespace DB
{
	class TextDB : public DB::ReadingDB
	{
	public:
		typedef struct
		{
			const UTF8Char *name;
			Data::ArrayList<const UTF8Char *> *colList;
			Data::ArrayList<const UTF8Char **> *valList;
		} DBData;

	private:
		Data::StringUTF8Map<DBData*> *dbMap;
		DBData *currDB;
	public:
		TextDB(const UTF8Char *sourceName);
		virtual ~TextDB();

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names); // no need to release
		virtual DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition);
		virtual void CloseReader(DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF *str);
		virtual void Reconnect();

		Bool AddTable(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *colList);
		Bool AddTableData(Data::ArrayList<const UTF8Char*> *valList);
	};
}
#endif
