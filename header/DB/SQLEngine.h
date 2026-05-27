#ifndef _SM_DB_SQLENGINE
#define _SM_DB_SQLENGINE
#include "DB/DBConn.h"

namespace DB
{
	class SQLEngine : public DBConn
	{
	private:
		DB::SQLType sqlType;
		Int8 tzQhr;
		Optional<Text::String> lastErrorMsg;
	public:
		SQLEngine(DB::SQLType sqlType, Text::CStringNN sourceName);
		virtual ~SQLEngine();

		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual IntOS ExecuteNonQuery(Text::CStringNN sql);
		virtual Optional<DBReader> ExecuteReader(Text::CStringNN sql);
		virtual Bool IsLastDataError();
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);

		virtual Optional<DBTransaction> BeginTransaction();
		virtual void Commit(NN<DBTransaction> tran);
		virtual void Rollback(NN<DBTransaction> tran);
	};
}
#endif
