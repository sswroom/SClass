#ifndef _SM_DB_WORKBOOKDB
#define _SM_DB_WORKBOOKDB
#include "DB/ReadingDB.h"
#include "Text/SpreadSheet/Workbook.h"

namespace DB
{
	class WorkbookDB : public DB::ReadingDB
	{
	private:
		Text::SpreadSheet::Workbook *wb;
	public:
		WorkbookDB(Text::SpreadSheet::Workbook *wb);
		virtual ~WorkbookDB();

		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names);
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(DBReader *r);
		virtual void GetLastErrorMsg(Text::StringBuilderUTF8 *str);
		virtual void Reconnect();
	};
}
#endif