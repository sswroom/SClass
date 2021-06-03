#ifndef _SM_DB_MDBFILE
#define _SM_DB_MDBFILE
#include "DB/DBTool.h"
#include "DB/ODBCConn.h"

namespace DB
{
	class MDBFile : public DB::ODBCConn
	{
	public:
		MDBFile(const UTF8Char *fileName, IO::LogTool *log, UInt32 codePage, const WChar *uid, const WChar *pwd);
	public:
		static Bool CreateMDBFile(const UTF8Char *fileName);
		static DBTool *CreateDBTool(const UTF8Char *fileName, IO::LogTool *log, const UTF8Char *logPrefix);
	};
}
#endif
