#ifndef _SM_DB_SQLBUILDER
#define _SM_DB_SQLBUILDER
#include "Data/DateTime.h"
#include "DB/DBUtil.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class TableDef;
	class ReadingDBTool;
	class DBQueue;
	
	class SQLBuilder
	{
	private:
		Text::StringBuilderUTF8 *sb;
		DB::DBUtil::ServerType svrType;
		Int32 tzQhr;

	public:
		SQLBuilder(DB::DBUtil::ServerType svrType, Int32 tzQhr);
		SQLBuilder(DB::ReadingDBTool *db);
		~SQLBuilder();

		void AppendCmd(const UTF8Char *val);
		void AppendInt32(Int32 val);
		void AppendInt64(Int64 val);
		void AppendUInt32(UInt32 val);
		void AppendUInt64(UInt64 val);
		void AppendStr(Text::String *val);
		void AppendStrUTF8(const UTF8Char *val);
		void AppendStrW(const WChar *val);
		void AppendDate(Data::DateTime *val);
		void AppendDbl(Double val);
		void AppendBool(Bool val);
		void AppendVector(Math::Vector2D *vec);
		void AppendBinary(const UInt8 *buff, UOSInt buffSize);

		void AppendTableName(DB::TableDef *table);
		void AppendCol(const UTF8Char *val);
		void AppendCol(const WChar *val);
		void AppendTrim(const UTF8Char *val);

		void Clear();
		const UTF8Char *ToString();
		UOSInt GetLength();
		Text::String *ToNewString();
	};
}
#endif
