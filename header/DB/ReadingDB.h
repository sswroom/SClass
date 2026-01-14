#ifndef _SM_DB_READINGDB
#define _SM_DB_READINGDB
#include "Data/ArrayListStringNN.h"
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
		ReadingDB(NN<Text::String> sourceName);
		ReadingDB(Text::CStringNN sourceName);
		virtual ~ReadingDB();

		virtual UOSInt QuerySchemaNames(NN<Data::ArrayListStringNN> names);
		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names) = 0; //Need Release
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition) = 0;
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName) = 0;
		virtual void CloseReader(NN<DBReader> r) = 0;
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str) = 0;
		virtual void Reconnect() = 0;
		virtual Int8 GetTzQhr() const;

		virtual UOSInt GetDatabaseNames(NN<Data::ArrayListStringNN> arr);
		virtual void ReleaseDatabaseNames(NN<Data::ArrayListStringNN> arr);
		virtual Bool ChangeDatabase(Text::CStringNN databaseName);
		virtual Optional<Text::String> GetCurrDBName();

		virtual IO::ParserType GetParserType() const;

		virtual Bool IsFullConn() const; //false = read only, true = DBConn
		virtual Bool IsDBTool() const; //true = ReadingDBTool

		static Bool IsDBObj(NN<IO::ParsedObject> pobj);
	};
}
#endif
