#ifndef _SM_DB_DBCONN
#define _SM_DB_DBCONN
#include "DB/DBUtil.h"
#include "DB/ReadingDB.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	struct DBTransaction;

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
			CT_MYSQLTCP,
			CT_POSTGRESQL,
			CT_TDSCONN
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
		DBConn(NN<Text::String> sourceName);
		DBConn(Text::CStringNN sourceName);
	public:
		virtual ~DBConn();

		virtual DB::SQLType GetSQLType() const = 0;
		virtual Bool IsAxisAware() const;
		virtual ConnType GetConnType() const = 0;
		virtual Int8 GetTzQhr() const = 0;
		virtual void ForceTz(Int8 tzQhr) = 0;
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb) = 0;
		virtual void Close() = 0;
		virtual IntOS ExecuteNonQuery(Text::CStringNN sql) = 0;
		virtual Optional<DBReader> ExecuteReader(Text::CStringNN sql) = 0;
		virtual Bool IsLastDataError() = 0;
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		DataError GetLastDataError();

		virtual Optional<DBTransaction> BeginTransaction() = 0;
		virtual void Commit(NN<DBTransaction> tran) = 0;
		virtual void Rollback(NN<DBTransaction> tran) = 0;

		virtual Bool IsFullConn() const;
	};
}
#endif
