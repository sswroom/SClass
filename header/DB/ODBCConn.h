#ifndef _SM_DB_ODBCCONN
#define _SM_DB_ODBCCONN
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "IO/ConfigFile.h"
#include "IO/LogTool.h"
#include "Text/StringBuilderUTF.h"

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
		DBUtil::ServerType svrType;
	private:
		void *envHand;
		void *connHand;
		void *lastStmtHand;
		Int32 lastStmtState;
		const UTF8Char *lastErrorMsg;
		ConnError connErr;
		Bool isTran;
		const UTF8Char *dsn;
		const UTF8Char *uid;
		const UTF8Char *pwd;
		const UTF8Char *schema;
		const UTF8Char *connStr;
		IO::LogTool *log;
		Bool enableDebug;
		Bool forceTz;
		Int8 tzQhr;
		Data::ArrayList<const UTF8Char *> *tableNames;

	private:
		void PrintError();
		void UpdateConnInfo();
		Bool Connect(const UTF8Char *dsn, const UTF8Char *uid, const UTF8Char *pwd, const UTF8Char *schema);

	protected:		
		Bool Connect(const UTF8Char *connStr);
		ODBCConn(const UTF8Char *sourceName, IO::LogTool *log);
	public:
		ODBCConn(const UTF8Char *dsn, const UTF8Char *uid, const UTF8Char *pwd, const UTF8Char *schema, IO::LogTool *log);
		ODBCConn(const UTF8Char *connStr, const UTF8Char *sourceName, IO::LogTool *log);
		virtual ~ODBCConn();

		virtual DB::DBUtil::ServerType GetSvrType();
		virtual ConnType GetConnType();
		virtual Int8 GetTzQhr();
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(Text::StringBuilderUTF *sb);
		virtual void Close();
		void Dispose();
		virtual OSInt ExecuteNonQuery(const UTF8Char *sql);
//		virtual OSInt ExecuteNonQuery(const WChar *sql);
		virtual DB::DBReader *ExecuteReader(const UTF8Char *sql);
//		virtual DB::DBReader *ExecuteReader(const WChar *sql);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF *str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		ConnError GetConnError();
		void SetEnableDebug(Bool enableDebug);
		void SetTraceFile(const WChar *fileName);
		UTF8Char *ShowTablesCmd(UTF8Char *sbuff);

		DBReader *GetTablesInfo();
		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names);
		virtual DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition);

	public:
		void ShowSQLError(const UTF16Char *state, const UTF16Char *errMsg);
		void LogSQLError(void *hStmt);

		const UTF8Char *GetConnStr();
		const UTF8Char *GetConnDSN();
		const UTF8Char *GetConnUID();
		const UTF8Char *GetConnPWD();
		const UTF8Char *GetConnSchema();

		static UOSInt GetDriverList(Data::ArrayList<const UTF8Char*> *driverList);
		static IO::ConfigFile *GetDriverInfo(const UTF8Char *driverName);
		static DBTool *CreateDBTool(const UTF8Char *dsn, const UTF8Char *uid, const UTF8Char *pwd, const UTF8Char *schema, IO::LogTool *log, const UTF8Char *logPrefix);
	};

	class ODBCReader : public DB::DBReader
	{
	private:
		typedef struct
		{
			Int16 odbcType;
			DB::DBUtil::ColType colType;
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
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb);
		virtual const UTF8Char *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Vector2D *GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual void DelNewStr(const UTF8Char *s);

		DB::DBUtil::ColType ODBCType2DBType(Int16 odbcType, UOSInt colSize);
	};
}
#endif
