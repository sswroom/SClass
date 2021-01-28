#ifndef _SM_DB_READINGDB
#define _SM_DB_READINGDB
#include "IO/ParsedObject.h"
#include "Data/ArrayList.h"
#include "Text/StringBuilderUTF.h"

namespace DB
{
	class ColDef;
	class DBReader;
	
	class ReadingDB : public IO::ParsedObject
	{
	public:
		ReadingDB(const UTF8Char *sourceName);
		virtual ~ReadingDB();

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names) = 0; // no need to release
		virtual DBReader *GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition) = 0;
		virtual void CloseReader(DBReader *r) = 0;
		virtual void GetErrorMsg(Text::StringBuilderUTF *str) = 0;
		virtual void Reconnect() = 0;

		virtual IO::ParsedObject::ParserType GetParserType();

		virtual Bool IsFullConn(); //false = read only, true = DBConn

		static Bool IsDBObj(IO::ParsedObject *pobj);
	};
}
#endif
