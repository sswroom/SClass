#ifndef _SM_DB_SQLBUILDER
#define _SM_DB_SQLBUILDER
#include "Data/DateTime.h"
#include "DB/DBUtil.h"
#include "DB/SQL/SQLCommand.h"
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
		SQLBuilder(NN<const DB::ReadingDBTool> db);
		~SQLBuilder();

		void AppendCmdSlow(UnsafeArrayOpt<const UTF8Char> val);
		void AppendCmdC(Text::CStringNN val);
		void AppendInt32(Int32 val);
		void AppendNInt32(NInt32 val);
		void AppendInt64(Int64 val);
		void AppendUInt32(UInt32 val);
		void AppendUInt64(UInt64 val);
		void AppendStr(Text::String *val);
		void AppendStr(Optional<Text::String> val);
		void AppendStr(NN<Text::String> val);
		void AppendStrC(Text::CString val);
		void AppendStrUTF8(UnsafeArrayOpt<const UTF8Char> val);
		void AppendStrW(const WChar *val);
		void AppendDateTime(Data::DateTime *val);
		void AppendTS(const Data::Timestamp &val);
		void AppendDate(const Data::Date &val);
		void AppendDbl(Double val);
		void AppendNDbl(Double val);
		void AppendBool(Bool val);
		void AppendVector(Optional<Math::Geometry::Vector2D> vec);
		void AppendBinary(UnsafeArrayOpt<const UInt8> buff, UIntOS buffSize);

		void AppendTableName(NN<DB::TableDef> table);
		void AppendCol(UnsafeArray<const UTF8Char> val);
		void AppendCol(const WChar *val);
		void AppendTrim(Text::CStringNN val);
		void AppendSQLCommand(NN<SQL::SQLCommand> cmd);

		void Clear();
		UnsafeArray<const UTF8Char> ToString() const;
		UIntOS GetLength() const;
		Text::CStringNN ToCString() const;
		NN<Text::String> ToNewString() const;
		Data::ByteArrayR ToByteArray() const;
		DB::SQLType GetSQLType() const;
		Bool SupportSchema() const;
	};
}
#endif
