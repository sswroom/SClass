#ifndef _SM_DB_READINGDB
#define _SM_DB_READINGDB
#include "Data/ArrayList.h"
#include "Data/QueryConditions.h"
#include "IO/ParsedObject.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class ColDef;
	class DBReader;
	
	class ReadingDB : public IO::ParsedObject
	{
	public:
		ReadingDB(Text::String *sourceName);
		ReadingDB(Text::CString sourceName);
		virtual ~ReadingDB();

		virtual UOSInt GetTableNames(Data::ArrayList<Text::CString> *names) = 0; // no need to release
		virtual DBReader *QueryTableData(Text::CString tableName, Data::ArrayList<Text::String*> *colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition) = 0;
		virtual void CloseReader(DBReader *r) = 0;
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str) = 0;
		virtual void Reconnect() = 0;

		virtual IO::ParserType GetParserType() const;

		virtual Bool IsFullConn(); //false = read only, true = DBConn

		static Bool IsDBObj(IO::ParsedObject *pobj);
	};
}
#endif
