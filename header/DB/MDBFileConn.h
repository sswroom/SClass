#ifndef _SM_DB_MDBFILECONN
#define _SM_DB_MDBFILECONN
#include "DB/DBTool.h"
#include "DB/ODBCConn.h"

namespace DB
{
	class MDBFileConn : public DB::ODBCConn
	{
	public:
		MDBFileConn(Text::CStringNN fileName, IO::LogTool *log, UInt32 codePage, const WChar *uid, const WChar *pwd);
	public:
		static Bool CreateMDBFile(Text::CStringNN fileName);
		static DBTool *CreateDBTool(NotNullPtr<Text::String> fileName, IO::LogTool *log, Text::CString logPrefix);
		static DBTool *CreateDBTool(Text::CStringNN fileName, IO::LogTool *log, Text::CString logPrefix);
	};
}
#endif
