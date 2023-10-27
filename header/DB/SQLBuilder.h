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
		Text::StringBuilderUTF8 sb;
		DB::SQLType sqlType;
		Int32 tzQhr;
		Bool axisAware;

	public:
		SQLBuilder(DB::SQLType sqlType, Bool axisAware, Int32 tzQhr);
		SQLBuilder(NotNullPtr<const DB::ReadingDBTool> db);
		~SQLBuilder();

		void AppendCmdSlow(const UTF8Char *val);
		void AppendCmdC(Text::CString val);
		void AppendInt32(Int32 val);
		void AppendInt64(Int64 val);
		void AppendUInt32(UInt32 val);
		void AppendUInt64(UInt64 val);
		void AppendStr(Text::String *val);
		void AppendStr(NotNullPtr<Text::String> val);
		void AppendStrC(Text::CString val);
		void AppendStrUTF8(const UTF8Char *val);
		void AppendStrW(const WChar *val);
		void AppendDateTime(Data::DateTime *val);
		void AppendTS(const Data::Timestamp &val);
		void AppendDate(const Data::Date &val);
		void AppendDbl(Double val);
		void AppendBool(Bool val);
		void AppendVector(Math::Geometry::Vector2D *vec);
		void AppendBinary(const UInt8 *buff, UOSInt buffSize);

		void AppendTableName(DB::TableDef *table);
		void AppendCol(const UTF8Char *val);
		void AppendCol(const WChar *val);
		void AppendTrim(Text::CString val);

		void Clear();
		const UTF8Char *ToString() const;
		UOSInt GetLength() const;
		Text::CStringNN ToCString() const;
		NotNullPtr<Text::String> ToNewString() const;
		DB::SQLType GetSQLType() const;
	};
}
#endif
