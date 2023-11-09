#ifndef _SM_DB_READINGDB
#define _SM_DB_READINGDB
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/QueryConditions.h"
#include "IO/ParsedObject.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class ColDef;
	class DBReader;
	class TableDef;
	
	class ReadingDB : public IO::ParsedObject
	{
	public:
		ReadingDB(NotNullPtr<Text::String> sourceName);
		ReadingDB(Text::CStringNN sourceName);
		virtual ~ReadingDB();

		virtual UOSInt QuerySchemaNames(NotNullPtr<Data::ArrayListNN<Text::String>> names);
		virtual UOSInt QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names) = 0; //Need Release
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition) = 0;
		virtual TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName) = 0;
		virtual void CloseReader(NotNullPtr<DBReader> r) = 0;
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str) = 0;
		virtual void Reconnect() = 0;
		virtual Int8 GetTzQhr() const;

		virtual UOSInt GetDatabaseNames(NotNullPtr<Data::ArrayListNN<Text::String>> arr);
		virtual void ReleaseDatabaseNames(NotNullPtr<Data::ArrayListNN<Text::String>> arr);
		virtual Bool ChangeDatabase(Text::CString databaseName);
		virtual Text::String *GetCurrDBName();

		virtual IO::ParserType GetParserType() const;

		virtual Bool IsFullConn() const; //false = read only, true = DBConn
		virtual Bool IsDBTool() const; //true = ReadingDBTool

		static Bool IsDBObj(NotNullPtr<IO::ParsedObject> pobj);
	};
}
#endif
