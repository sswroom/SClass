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
		ReadingDB(Text::CString sourceName);
		virtual ~ReadingDB();

		virtual UOSInt QuerySchemaNames(Data::ArrayList<Text::String*> *names);
		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names) = 0; //Need Release
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition) = 0;
		virtual TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName) = 0;
		virtual void CloseReader(DBReader *r) = 0;
		virtual void GetLastErrorMsg(Text::StringBuilderUTF8 *str) = 0;
		virtual void Reconnect() = 0;

		virtual UOSInt GetDatabaseNames(Data::ArrayList<Text::String*> *arr);
		virtual void ReleaseDatabaseNames(Data::ArrayList<Text::String*> *arr);
		virtual Bool ChangeDatabase(Text::CString databaseName);
		virtual Text::String *GetCurrDBName();

		virtual IO::ParserType GetParserType() const;

		virtual Bool IsFullConn(); //false = read only, true = DBConn
		virtual Bool IsDBTool(); //true = ReadingDBTool

		static Bool IsDBObj(IO::ParsedObject *pobj);
	};
}
#endif
