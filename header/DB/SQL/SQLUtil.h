#ifndef _SM_DB_SQLUTIL
#define _SM_DB_SQLUTIL
#include "DB/DBUtil.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	namespace SQL
	{
		class SQLUtil
		{
		public:
			static const UTF8Char *ParseNextWord(const UTF8Char *sql, NotNullPtr<Text::StringBuilderUTF8> sb, DB::SQLType sqlType);
			static Data::VariItem *ParseValue(Text::CStringNN val, DB::SQLType sqlType);
		};
	}
}
#endif
