#ifndef _SM_DB_READINGDB
#define _SM_DB_READINGDB
#include "Data/ArrayList.h"
#include "Data/QueryConditions.h"
#include "IO/ParsedObject.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF.h"

namespace DB
{
	class ColDef;
	class DBReader;
	
	class ReadingDB : public IO::ParsedObject
	{
	public:
		ReadingDB(Text::String *sourceName);
		ReadingDB(const UTF8Char *sourceName);
		virtual ~ReadingDB();

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names) = 0; // no need to release
		virtual DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<Text::String*> *colNames, UOSInt dataOfst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition) = 0;
		virtual void CloseReader(DBReader *r) = 0;
		virtual void GetErrorMsg(Text::StringBuilderUTF *str) = 0;
		virtual void Reconnect() = 0;

		virtual IO::ParserType GetParserType();

		virtual Bool IsFullConn(); //false = read only, true = DBConn

		static Bool IsDBObj(IO::ParsedObject *pobj);
	};
}
#endif
