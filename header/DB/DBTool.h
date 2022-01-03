#ifndef _SM_DB_DBTOOL
#define _SM_DB_DBTOOL
#include "DB/DBConn.h"
#include "DB/PageRequest.h"
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

		typedef enum
		{
			PS_SUCC,
			PS_NO_OFFSET,
			PS_NO_PAGE
		} PageStatus;

	private:
		Int32 nqFail;

		void *tran;

	public:
		DBTool(DBConn *conn, Bool needRelease, IO::LogTool *log, const UTF8Char *logPrefix);
		virtual ~DBTool();

/*		static DBTool *NullSource(IO::LogTool *);*/

		OSInt ExecuteNonQuery(const UTF8Char *sqlCmd);
		OSInt ExecuteNonQueryC(const UTF8Char *sqlCmd, UOSInt len);
		void BeginTrans();
		void EndTrans(Bool toCommit);
		Int32 GetLastIdentity32();
		Int64 GetLastIdentity64();
		DB::DBConn *GetConn();
		
		Bool GenCreateTableCmd(DB::SQLBuilder *sql, const UTF8Char *tableName, DB::TableDef *tabDef);
		Bool GenDropTableCmd(DB::SQLBuilder *sql, const UTF8Char *tableName);
		Bool GenDeleteTableCmd(DB::SQLBuilder *sql, const UTF8Char *tableName);
		PageStatus GenSelectCmdPage(DB::SQLBuilder *sql, DB::TableDef *tabDef, DB::PageRequest *page);
		Bool GenInsertCmd(DB::SQLBuilder *sql, const UTF8Char *tableName, DB::DBReader *r);
		UTF8Char *GenInsertCmd(UTF8Char *sqlstr, const UTF8Char *tableName, DB::DBReader *r);
	};
}
#endif
