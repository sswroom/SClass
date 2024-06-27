#ifndef _SM_DB_ODBCCONN
#define _SM_DB_ODBCCONN
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "IO/ConfigFile.h"
#include "IO/LogTool.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class ODBCConn : public DB::DBConn
	{
	public:
		typedef enum
		{
			CE_NONE,
			CE_NOT_CONNECT,
			CE_ALLOC_ENV,
			CE_SET_ENV,
			CE_ALLOC_DBC,
			CE_SET_DBC,
			CE_CONNECT_ERR
		} ConnError;
	protected:
		SQLType sqlType;
	private:
		void *envHand;
		void *connHand;
		void *lastStmtHand;
		Int32 lastStmtState;
		Text::String *lastErrorMsg;
		ConnError connErr;
		Bool isTran;
		Optional<Text::String> dsn;
		Optional<Text::String> uid;
		Optional<Text::String> pwd;
		Optional<Text::String> schema;
		Text::String *connStr;
		NN<IO::LogTool> log;
		Bool enableDebug;
		Bool forceTz;
		Int8 tzQhr;
		Bool axisAware;

	private:
		void PrintError();
		void UpdateConnInfo();
		Bool Connect(Optional<Text::String> dsn, Optional<Text::String> uid, Optional<Text::String> pwd, Optional<Text::String> schema);

	protected:		
		Bool Connect(NN<Text::String> connStr);
		Bool Connect(Text::CStringNN connStr);
		ODBCConn(Text::CStringNN sourceName, NN<IO::LogTool> log);
	public:
		ODBCConn(NN<Text::String> dsn, Optional<Text::String> uid, Optional<Text::String> pwd, Optional<Text::String> schema, NN<IO::LogTool> log);
		ODBCConn(Text::CStringNN dsn, Text::CString uid, Text::CString pwd, Text::CString schema, NN<IO::LogTool> log);
		ODBCConn(Text::CStringNN connStr, Text::CStringNN sourceName, NN<IO::LogTool> log);
		virtual ~ODBCConn();

		virtual DB::SQLType GetSQLType() const;
		virtual Bool IsAxisAware() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
		virtual void Close();
		void Dispose();
		virtual OSInt ExecuteNonQuery(Text::CStringNN sql);
		virtual Optional<DB::DBReader> ExecuteReader(Text::CStringNN sql);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual Optional<DB::DBTransaction> BeginTransaction();
		virtual void Commit(NN<DB::DBTransaction> tran);
		virtual void Rollback(NN<DB::DBTransaction> tran);

		ConnError GetConnError();
		void SetEnableDebug(Bool enableDebug);
		void SetTraceFile(const WChar *fileName);
		UnsafeArray<UTF8Char> ShowTablesCmd(UnsafeArray<UTF8Char> sbuff);

		Optional<DBReader> GetTablesInfo(Text::CString schemaName);
		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);

	public:
		void ShowSQLError(const UTF16Char *state, const UTF16Char *errMsg);
		void LogSQLError(void *hStmt);

		Text::String *GetConnStr();
		Optional<Text::String> GetConnDSN();
		Optional<Text::String> GetConnUID();
		Optional<Text::String> GetConnPWD();
		Optional<Text::String> GetConnSchema();

		static UOSInt GetDriverList(NN<Data::ArrayListStringNN> driverList);
		static Optional<IO::ConfigFile> GetDriverInfo(Text::CString driverName);
		static Optional<DBTool> CreateDBTool(NN<Text::String> dsn, Optional<Text::String> uid, Optional<Text::String> pwd, Optional<Text::String> schema, NN<IO::LogTool> log, Text::CString logPrefix);
		static Optional<DBTool> CreateDBTool(Text::CStringNN dsn, Text::CString uid, Text::CString pwd, Text::CString schema, NN<IO::LogTool> log, Text::CString logPrefix);
	};

	class ODBCReader : public DB::DBReader
	{
	private:
		typedef struct
		{
			Int16 odbcType;
			DB::DBUtil::ColType colType;
			UOSInt colSize;
			void *colData;
			Int64 dataVal;
			Bool isNull;
		} ColumnData;
	private:
		ODBCConn *conn;
		UOSInt colCnt;
		ColumnData *colDatas;
		Bool enableDebug;
		OSInt rowChanged;
		Int8 tzQhr;
		void *hStmt;

	public:
		ODBCReader(ODBCConn *conn, void *hStmt, Bool enableDebug, Int8 tzQhr);
		~ODBCReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);
		virtual Bool GetVariItem(UOSInt colIndex, NN<Data::VariItem> item);

		virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);

		DB::DBUtil::ColType ODBCType2DBType(Int16 odbcType, UOSInt colSize);
	};
}
#endif
