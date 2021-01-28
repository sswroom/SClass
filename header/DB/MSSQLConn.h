#ifndef _SM_DB_MDBFILE
#define _SM_DB_MDBFILE
#include "DB/DBTool.h"
#include "Text/StringBuilderUTF.h"

namespace DB
{
	class MSSQLConn
	{
	public:
		static DB::DBConn *OpenConnTCP(const UTF8Char *serverHost, UInt16 port, const UTF8Char *database, const UTF8Char *userName, const UTF8Char *password, IO::LogTool *log, Text::StringBuilderUTF *errMsg);
		static DB::DBTool *CreateDBToolTCP(const UTF8Char *serverHost, UInt16 port, const UTF8Char *database, const UTF8Char *userName, const UTF8Char *password, IO::LogTool *log, Bool useMut, const UTF8Char *logPrefix);
		static const UTF8Char *GetDriverName();
	};
}
#endif
