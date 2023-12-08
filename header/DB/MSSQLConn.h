#ifndef _SM_DB_MDBFILE
#define _SM_DB_MDBFILE
#include "DB/DBTool.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class MSSQLConn
	{
	public:
		static Optional<DB::DBConn> OpenConnTCP(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, NotNullPtr<IO::LogTool> log, Text::StringBuilderUTF8 *errMsg);
		static Optional<DB::DBTool> CreateDBToolTCP(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);
		static Text::String *GetDriverNameNew();
		static Bool IsNative();
	};
}
#endif
