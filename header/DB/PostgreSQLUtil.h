#ifndef _SM_DB_POSTGRESQLUTIL
#define _SM_DB_POSTGRESQLUTIL
#include "DB/DBTool.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class PostgreSQLUtil
	{
	public:
		static Optional<DB::DBConn> OpenConn(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, NN<IO::LogTool> log, Optional<Text::StringBuilderUTF8> errMsg, Bool continueOnConnError = false);
		static Optional<DB::DBTool> CreateDBTool(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, NN<IO::LogTool> log, Text::CString logPrefix, Bool continueOnConnError = false);
		static Optional<Text::String> GetDriverNameNew();
	};
}
#endif
