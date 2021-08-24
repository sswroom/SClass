#ifndef _SM_DB_DBCONN
#define _SM_DB_DBCONN
#include "DB/DBUtil.h"
#include "DB/ReadingDB.h"
#include "Text/StringBuilderUTF.h"

namespace DB
{
	class DBConn : public DB::ReadingDB
	{
	public:
		typedef enum
		{
			CT_UNKNOWN = 0,
			CT_ODBC,
			CT_MYSQL,
			CT_SQLITE,
			CT_WMIQUERY,
			CT_OLEDB,
			CT_MYSQLTCP
		} ConnType;

		typedef enum
		{
			DE_NO_ERROR,
			DE_CONN_ERROR,
			DE_NO_CONN,
			DE_INIT_SQL_ERROR,
			DE_EXEC_SQL_ERROR
		} DataError;

	protected:
		DataError lastDataError;
		DBConn(const UTF8Char *sourceName);
	public:
		virtual ~DBConn();

		virtual DB::DBUtil::ServerType GetSvrType() = 0;
		virtual ConnType GetConnType() = 0;
		virtual Int8 GetTzQhr() = 0;
		virtual void ForceTz(Int8 tzQhr) = 0;
		virtual void GetConnName(Text::StringBuilderUTF *sb) = 0;
		virtual void Close() = 0;
		virtual OSInt ExecuteNonQuery(const UTF8Char *sql) = 0;
//		virtual OSInt ExecuteNonQuery(const WChar *sql) = 0;
		virtual DBReader *ExecuteReader(const UTF8Char *sql) = 0;
//		virtual DBReader *ExecuteReader(const WChar *sql) = 0;
		virtual Bool IsLastDataError() = 0;
		DataError GetLastDataError();

		virtual void *BeginTransaction() = 0;
		virtual void Commit(void *tran) = 0;
		virtual void Rollback(void *tran) = 0;

		virtual Bool IsFullConn();
	};
}
#endif
