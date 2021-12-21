#ifndef _SM_DB_MDBFILECONN
#define _SM_DB_MDBFILECONN
#include "DB/DBTool.h"
#include "DB/ODBCConn.h"

namespace DB
{
	class MDBFileConn : public DB::ODBCConn
	{
	public:
		MDBFileConn(const UTF8Char *fileName, IO::LogTool *log, UInt32 codePage, const WChar *uid, const WChar *pwd);
	public:
		static Bool CreateMDBFile(const UTF8Char *fileName);
		static DBTool *CreateDBTool(Text::String *fileName, IO::LogTool *log, const UTF8Char *logPrefix);
		static DBTool *CreateDBTool(const UTF8Char *fileName, IO::LogTool *log, const UTF8Char *logPrefix);
	};
}
#endif
