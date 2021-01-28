#ifndef _SM_DB_SQLBUILDER
#define _SM_DB_SQLBUILDER
#include "Data/DateTime.h"
#include "DB/DBUtil.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class SQLBuilder
	{
	private:
		Text::StringBuilderUTF8 *sb;
		DB::DBUtil::ServerType svrType;

	public:
		SQLBuilder(DB::DBUtil::ServerType svrType);
		~SQLBuilder();

		void AppendCmd(const UTF8Char *val);
		void AppendInt32(Int32 val);
		void AppendInt64(Int64 val);
		void AppendStrUTF8(const UTF8Char *val);
		void AppendStrW(const WChar *val);
		void AppendDate(Data::DateTime *val);
		void AppendDbl(Double val);
		void AppendBool(Bool val);
		void AppendVector(Math::Vector2D *vec, Int32 srId);
		void AppendBinary(const UInt8 *buff, OSInt buffSize);

		void AppendCol(const UTF8Char *val);
		void AppendCol(const WChar *val);
		void AppendTrim(const UTF8Char *val);

		void Clear();
		const UTF8Char *ToString();
	};
}
#endif
