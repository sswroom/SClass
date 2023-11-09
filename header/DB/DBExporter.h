#ifndef _SM_DB_DBEXPORTER
#define _SM_DB_DBEXPORTER
#include "DB/ReadingDB.h"
#include "DB/SQLiteFile.h"
#include "IO/SeekableStream.h"
#include "Text/SpreadSheet/Workbook.h"

namespace DB
{
	class DBExporter
	{
	public:
		static Data::Class *CreateTableClass(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName);
		static Bool GenerateInsertSQLs(NotNullPtr<DB::ReadingDB> db, DB::SQLType sqlType, Bool axisAware, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<IO::Stream> outStm);
		static Bool GenerateCSV(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, Text::CStringNN nullText, NotNullPtr<IO::Stream> outStm, UInt32 codePage);
		static Bool GenerateSQLite(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<DB::SQLiteFile> file, Text::StringBuilderUTF8 *sbError);
		static Bool GenerateHTML(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<IO::Stream> outStm, UInt32 codePage);
		static Bool GeneratePList(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<IO::Stream> outStm, UInt32 codePage);
		static Bool AppendWorksheet(NotNullPtr<Text::SpreadSheet::Workbook> wb, NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, Text::StringBuilderUTF8 *sbError);
		static Bool GenerateXLSX(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<IO::SeekableStream> outStm, Text::StringBuilderUTF8 *sbError);
		static Bool GenerateExcelXML(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<IO::SeekableStream> outStm, Text::StringBuilderUTF8 *sbError);

		static Bool GenerateExcelXMLAllTables(NotNullPtr<DB::ReadingDB> db, Text::CString schema, NotNullPtr<IO::Stream> outStm, UInt32 codePage);
	};
}
#endif
