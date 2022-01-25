#ifndef _SM_DB_MDBFILE
#define _SM_DB_MDBFILE
#include "DB/DBTool.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class MSSQLConn
	{
	public:
		static DB::DBConn *OpenConnTCP(const UTF8Char *serverHost, UInt16 port, const UTF8Char *database, const UTF8Char *userName, const UTF8Char *password, IO::LogTool *log, Text::StringBuilderUTF8 *errMsg);
		static DB::DBTool *CreateDBToolTCP(const UTF8Char *serverHost, UInt16 port, const UTF8Char *database, const UTF8Char *userName, const UTF8Char *password, IO::LogTool *log, const UTF8Char *logPrefix);
		static Text::String *GetDriverNameNew();
	};
}
#endif
