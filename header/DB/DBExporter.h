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
		static Optional<Data::Class> CreateTableClass(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName);
		static Bool GenerateInsertSQLs(NN<DB::ReadingDB> db, DB::SQLType sqlType, Bool axisAware, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<IO::Stream> outStm);
		static Bool GenerateCSV(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, Text::CStringNN nullText, NN<IO::Stream> outStm, UInt32 codePage);
		static Bool GenerateSQLite(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<DB::SQLiteFile> file, Optional<Text::StringBuilderUTF8> sbError);
		static Bool GenerateHTML(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<IO::Stream> outStm, UInt32 codePage);
		static Bool GeneratePList(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<IO::Stream> outStm, UInt32 codePage);
		static Bool AppendWorksheet(NN<Text::SpreadSheet::Workbook> wb, NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, Optional<Text::StringBuilderUTF8> sbError);
		static Bool GenerateXLSX(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<IO::SeekableStream> outStm, Optional<Text::StringBuilderUTF8> sbError);
		static Bool GenerateExcelXML(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<IO::SeekableStream> outStm, Optional<Text::StringBuilderUTF8> sbError);

		static Bool GenerateExcelXMLAllTables(NN<DB::ReadingDB> db, Text::CString schema, NN<IO::Stream> outStm, UInt32 codePage);
	};
}
#endif
