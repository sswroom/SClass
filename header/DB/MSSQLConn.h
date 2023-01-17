#ifndef _SM_DB_MDBFILE
#define _SM_DB_MDBFILE
#include "DB/DBTool.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class MSSQLConn
	{
	public:
		static DB::DBConn *OpenConnTCP(Text::CString serverHost, UInt16 port, Text::CString database, Text::CString userName, Text::CString password, IO::LogTool *log, Text::StringBuilderUTF8 *errMsg);
		static DB::DBTool *CreateDBToolTCP(Text::CString serverHost, UInt16 port, Text::CString database, Text::CString userName, Text::CString password, IO::LogTool *log, Text::CString logPrefix);
		static Text::String *GetDriverNameNew();
	};
}
#endif
