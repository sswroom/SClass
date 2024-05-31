#ifndef _SM_DB_WORKBOOKDB
#define _SM_DB_WORKBOOKDB
#include "DB/ReadingDB.h"
#include "Text/SpreadSheet/Workbook.h"

namespace DB
{
	class WorkbookDB : public DB::ReadingDB
	{
	private:
		NN<Text::SpreadSheet::Workbook> wb;
	public:
		WorkbookDB(NN<Text::SpreadSheet::Workbook> wb);
		virtual ~WorkbookDB();

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Data::ArrayListStringNN *colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
	};
}
#endif
