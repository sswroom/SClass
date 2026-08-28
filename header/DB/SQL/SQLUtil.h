#ifndef _SM_DB_SQL_SQLUTIL
#define _SM_DB_SQL_SQLUTIL
#include "DB/DBUtil.h"
#include "DB/SQL/SQLValue.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	namespace SQL
	{
		class SQLUtil
		{
		public:
			static UnsafeArray<const UTF8Char> ParseNextWord(UnsafeArray<const UTF8Char> sql, NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType);
			static Optional<SQLValue> ParseNativeValue(Text::CStringNN val, DB::SQLType sqlType);
			static Optional<SQLValue> ParseValueAndNext(InOutParam<UnsafeArray<const UTF8Char>> sql, NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType);
			static void ParseColumnWord(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType);
		};
	}
}
#endif
