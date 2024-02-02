#ifndef _SM_DB_DBTOOL
#define _SM_DB_DBTOOL
#include "DB/DBConn.h"
#include "DB/ReadingDBTool.h"
#include "Net/SSHManager.h"

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
		Optional<Net::SSHManager> ssh;
		Optional<Net::SSHClient> sshCli;

	public:
		DBTool(NotNullPtr<DBConn> conn, Bool needRelease, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);
		virtual ~DBTool();

/*		static DBTool *NullSource(IO::LogTool *);*/
		void SetSSHTunnel(Optional<Net::SSHManager> ssh, Optional<Net::SSHClient> sshCli);
		Optional<Net::SSHClient> GetSSHClient() const;

		OSInt ExecuteNonQuery(Text::CStringNN sqlCmd);
		void BeginTrans();
		void EndTrans(Bool toCommit);
		Int32 GetLastIdentity32();
		Int64 GetLastIdentity64();
		Bool CreateDatabase(Text::CString databaseName, const Collation *collation);
		Bool DeleteDatabase(Text::CString databaseName);
		Bool CreateSchema(Text::CString schemaName);
		Bool DeleteSchema(Text::CString schemaName);
		Bool DeleteTableData(Text::CString schemaName, Text::CString tableName);
	
		Bool KillConnection(Int32 id);

		virtual Bool CanModify();
	};
}
#endif
