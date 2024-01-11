#ifndef _SM_DB_POSTGRESQLCONN
#define _SM_DB_POSTGRESQLCONN
#include "Data/ArrayList.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "IO/LogTool.h"
#include "Net/SocketFactory.h"

namespace DB
{
	class PostgreSQLConn : public DB::DBConn
	{
	private:
		struct ClassData;
		ClassData *clsData;

		Bool isTran;
		NotNullPtr<Text::String> server;
		UInt16 port;
		NotNullPtr<Text::String> database;
		Optional<Text::String> uid;
		Optional<Text::String> pwd;
		NotNullPtr<IO::LogTool> log;
		Int8 tzQhr;
		UInt32 geometryOid;
		UInt32 citextOid;

		Bool Connect();
		void InitConnection();

	public:
		PostgreSQLConn(NotNullPtr<Text::String> server, UInt16 port, Text::String *uid, Text::String *pwd, NotNullPtr<Text::String> database, NotNullPtr<IO::LogTool> log);
		PostgreSQLConn(Text::CStringNN server, UInt16 port, Text::CString uid, Text::CString pwd, Text::CString database, NotNullPtr<IO::LogTool> log);
		virtual ~PostgreSQLConn();
		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual void Dispose();
		virtual OSInt ExecuteNonQuery(Text::CStringNN sql);
		virtual Optional<DBReader> ExecuteReader(Text::CStringNN sql);
		virtual void CloseReader(NotNullPtr<DB::DBReader> r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		virtual UOSInt QuerySchemaNames(NotNullPtr<Data::ArrayListStringNN> names);
		virtual UOSInt QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);

		Bool IsConnError();
		NotNullPtr<Text::String> GetConnServer() const;
		UInt16 GetConnPort() const;
		NotNullPtr<Text::String> GetConnDB() const;
		Optional<Text::String> GetConnUID() const;
		Optional<Text::String> GetConnPWD() const;
		Bool ChangeDatabase(Text::CString databaseName);

		UInt32 GetGeometryOid();
		UInt32 GetCitextOid();
		DB::DBUtil::ColType DBType2ColType(UInt32 dbType);

		static Text::CString ExecStatusTypeGetName(OSInt status);
		static Optional<DBTool> CreateDBTool(NotNullPtr<Text::String> serverName, UInt16 port, NotNullPtr<Text::String> dbName, Text::String *uid, Text::String *pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);
		static Optional<DBTool> CreateDBTool(Text::CStringNN serverName, UInt16 port, Text::CString dbName, Text::CString uid, Text::CString pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);
	};
}
#endif
