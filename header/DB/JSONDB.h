#ifndef _SM_DB_JSONDB
#define _SM_DB_JSONDB
#include "DB/ReadingDB.h"
#include "Text/JSON.h"

namespace DB
{
	class JSONDB : public DB::ReadingDB
	{
	private:
		Text::String *layerName;
		Text::JSONArray *data;
	public:
		JSONDB(Text::String *sourceName, Text::CString layerName, Text::JSONArray *data);
		virtual ~JSONDB();

		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names);
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(DBReader *r);
		virtual void GetLastErrorMsg(Text::StringBuilderUTF8 *str);
		virtual void Reconnect();
	};
}
#endif