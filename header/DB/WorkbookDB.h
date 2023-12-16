#ifndef _SM_DB_WORKBOOKDB
#define _SM_DB_WORKBOOKDB
#include "DB/ReadingDB.h"
#include "Text/SpreadSheet/Workbook.h"

namespace DB
{
	class WorkbookDB : public DB::ReadingDB
	{
	private:
		NotNullPtr<Text::SpreadSheet::Workbook> wb;
	public:
		WorkbookDB(NotNullPtr<Text::SpreadSheet::Workbook> wb);
		virtual ~WorkbookDB();

		virtual UOSInt QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names);
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(NotNullPtr<DBReader> r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
	};
}
#endif
