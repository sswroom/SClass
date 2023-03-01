#ifndef _SM_DB_DBTOOL
#define _SM_DB_DBTOOL
#include "DB/DBConn.h"
#include "DB/ReadingDBTool.h"

namespace DB
{
	class DBTool : public ReadingDBTool
	{
	public:
		typedef enum
		{
			ReaderTrigger = 0,
			NonQueryTrigger = 1
		} TriggerType;

	private:
		Int32 nqFail;

		void *tran;

	public:
		DBTool(DBConn *conn, Bool needRelease, IO::LogTool *log, Text::CString logPrefix);
		virtual ~DBTool();

/*		static DBTool *NullSource(IO::LogTool *);*/

		OSInt ExecuteNonQuery(Text::CString sqlCmd);
		void BeginTrans();
		void EndTrans(Bool toCommit);
		Int32 GetLastIdentity32();
		Int64 GetLastIdentity64();
		Bool CreateDatabase(Text::CString databaseName);
		Bool DeleteDatabase(Text::CString databaseName);
		Bool CreateSchema(Text::CString schemaName);
		Bool DeleteSchema(Text::CString schemaName);
		Bool DeleteTableData(Text::CString schemaName, Text::CString tableName);
	
		Bool KillConnection(Int32 id);

		virtual Bool CanModify();
	};
}
#endif
