#ifndef _SM_DB_POSTGRESQLCONN
#define _SM_DB_POSTGRESQLCONN
#include "Data/ArrayListStringNN.h"
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
		NN<ClassData> clsData;

		Bool isTran;
		NN<Text::String> server;
		UInt16 port;
		NN<Text::String> database;
		Optional<Text::String> uid;
		Optional<Text::String> pwd;
		NN<IO::LogTool> log;
		Int8 tzQhr;
		UInt32 geometryOid;
		UInt32 citextOid;
		UInt32 stgeometryOid;
		Bool lastDataError;

		Bool Connect();
		void InitConnection();

	public:
		PostgreSQLConn(NN<Text::String> server, UInt16 port, Optional<Text::String> uid, Optional<Text::String> pwd, NN<Text::String> database, NN<IO::LogTool> log);
		PostgreSQLConn(Text::CStringNN server, UInt16 port, Text::CString uid, Text::CString pwd, Text::CStringNN database, NN<IO::LogTool> log);
		virtual ~PostgreSQLConn();
		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual void Dispose();
		virtual OSInt ExecuteNonQuery(Text::CStringNN sql);
		virtual Optional<DBReader> ExecuteReader(Text::CStringNN sql);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual Optional<DB::DBTransaction> BeginTransaction();
		virtual void Commit(NN<DB::DBTransaction> tran);
		virtual void Rollback(NN<DB::DBTransaction> tran);

		virtual UOSInt QuerySchemaNames(NN<Data::ArrayListStringNN> names);
		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);

		Bool IsConnError();
		NN<Text::String> GetConnServer() const;
		UInt16 GetConnPort() const;
		NN<Text::String> GetConnDB() const;
		Optional<Text::String> GetConnUID() const;
		Optional<Text::String> GetConnPWD() const;
		Bool ChangeDatabase(Text::CStringNN databaseName);

		UInt32 GetGeometryOid() const;
		UInt32 GetSTGeometryOid() const;
		UInt32 GetCitextOid() const;
		DB::DBUtil::ColType DBType2ColType(UInt32 dbType);

		static Text::CString ExecStatusTypeGetName(OSInt status);
		static Optional<DBTool> CreateDBTool(NN<Text::String> serverName, UInt16 port, NN<Text::String> dbName, Optional<Text::String> uid, Optional<Text::String> pwd, NN<IO::LogTool> log, Text::CString logPrefix);
		static Optional<DBTool> CreateDBTool(Text::CStringNN serverName, UInt16 port, Text::CStringNN dbName, Text::CString uid, Text::CString pwd, NN<IO::LogTool> log, Text::CString logPrefix);
	};
}
#endif
