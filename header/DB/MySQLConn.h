#ifndef _SM_DB_MYSQLCONN
#define _SM_DB_MYSQLCONN
#include "Data/ArrayList.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "IO/LogTool.h"
#include "Net/SocketFactory.h"

namespace DB
{
	class MySQLConn : public DB::DBConn
	{
	private:
		static Int32 useCnt;

		void *mysql;

		Bool axisAware;
		Bool isTran;
		NN<Text::String> server;
		Optional<Text::String> database;
		Optional<Text::String> uid;
		Optional<Text::String> pwd;
		NN<IO::LogTool> log;

		void Connect();

	public:
		MySQLConn(NN<Text::String> server, Text::String *uid, Text::String *pwd, Text::String *database, NN<IO::LogTool> log);
		MySQLConn(Text::CStringNN server, Text::CString uid, Text::CString pwd, Text::CString database, NN<IO::LogTool> log);
		MySQLConn(const WChar *server, const WChar *uid, const WChar *pwd, const WChar *database, NN<IO::LogTool> log);
		virtual ~MySQLConn();
		virtual DB::SQLType GetSQLType() const;
		virtual Bool IsAxisAware() const;
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

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);

		Bool IsConnError();
		NN<Text::String> GetConnServer();
		Optional<Text::String> GetConnDB();
		Optional<Text::String> GetConnUID();
		Optional<Text::String> GetConnPWD();

//		static DBTool *CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, NN<IO::LogTool> log);
		static DBTool *CreateDBTool(NN<Net::SocketFactory> sockf, NN<Text::String> serverName, Text::String *dbName, Text::String *uid, Text::String *pwd, NN<IO::LogTool> log, Text::CString logPrefix);
		static DBTool *CreateDBTool(NN<Net::SocketFactory> sockf, Text::CStringNN serverName, Text::CString dbName, Text::CString uid, Text::CString pwd, NN<IO::LogTool> log, Text::CString logPrefix);
//		static DBTool *CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, NN<IO::LogTool> log, Text::CString logPrefix);
	};

	class MySQLReader : public DB::DBReader
	{
	private:
		OSInt rowChanged;
		void *result;
		UOSInt colCount;
		Char **row;
		UInt32 *lengs;
		WChar **names;

	public:
		MySQLReader(OSInt rowChanged, void *result);
		virtual ~MySQLReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);

//		virtual WChar *GetName(OSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);
	
		DB::DBUtil::ColType ToColType(Int32 dbType, UInt32 flags, UOSInt colSize);
	};
}
#endif
