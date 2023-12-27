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
		NotNullPtr<Text::String> server;
		Optional<Text::String> database;
		Optional<Text::String> uid;
		Optional<Text::String> pwd;
		NotNullPtr<IO::LogTool> log;

		void Connect();

	public:
		MySQLConn(NotNullPtr<Text::String> server, Text::String *uid, Text::String *pwd, Text::String *database, NotNullPtr<IO::LogTool> log);
		MySQLConn(Text::CStringNN server, Text::CString uid, Text::CString pwd, Text::CString database, NotNullPtr<IO::LogTool> log);
		MySQLConn(const WChar *server, const WChar *uid, const WChar *pwd, const WChar *database, NotNullPtr<IO::LogTool> log);
		virtual ~MySQLConn();
		virtual DB::SQLType GetSQLType() const;
		virtual Bool IsAxisAware() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual void Dispose();
		virtual OSInt ExecuteNonQuery(Text::CStringNN sql);
//		virtual OSInt ExecuteNonQuery(const WChar *sql);
		virtual DBReader *ExecuteReader(Text::CStringNN sql);
//		virtual DBReader *ExecuteReader(const WChar *sql);
		virtual void CloseReader(NotNullPtr<DB::DBReader> r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListStringNN> names);
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);

		Bool IsConnError();
		NotNullPtr<Text::String> GetConnServer();
		Optional<Text::String> GetConnDB();
		Optional<Text::String> GetConnUID();
		Optional<Text::String> GetConnPWD();

//		static DBTool *CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, NotNullPtr<IO::LogTool> log);
		static DBTool *CreateDBTool(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<Text::String> serverName, Text::String *dbName, Text::String *uid, Text::String *pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);
		static DBTool *CreateDBTool(NotNullPtr<Net::SocketFactory> sockf, Text::CStringNN serverName, Text::CString dbName, Text::CString uid, Text::CString pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);
//		static DBTool *CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);
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
		virtual Bool GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid);

//		virtual WChar *GetName(OSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
	
		DB::DBUtil::ColType ToColType(Int32 dbType, UInt32 flags, UOSInt colSize);
	};
}
#endif
