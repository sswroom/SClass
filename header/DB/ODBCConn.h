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
		DBUtil::SQLType sqlType;
	private:
		void *envHand;
		void *connHand;
		void *lastStmtHand;
		Int32 lastStmtState;
		Text::String *lastErrorMsg;
		ConnError connErr;
		Bool isTran;
		Text::String *dsn;
		Text::String *uid;
		Text::String *pwd;
		Text::String *schema;
		Text::String *connStr;
		IO::LogTool *log;
		Bool enableDebug;
		Bool forceTz;
		Int8 tzQhr;

	private:
		void PrintError();
		void UpdateConnInfo();
		Bool Connect(Text::String *dsn, Text::String *uid, Text::String *pwd, Text::String *schema);

	protected:		
		Bool Connect(Text::String *connStr);
		Bool Connect(Text::CString connStr);
		ODBCConn(Text::CString sourceName, IO::LogTool *log);
	public:
		ODBCConn(Text::String *dsn, Text::String *uid, Text::String *pwd, Text::String *schema, IO::LogTool *log);
		ODBCConn(Text::CString dsn, Text::CString uid, Text::CString pwd, Text::CString schema, IO::LogTool *log);
		ODBCConn(Text::CString connStr, Text::CString sourceName, IO::LogTool *log);
		virtual ~ODBCConn();

		virtual DB::DBUtil::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(Text::StringBuilderUTF8 *sb);
		virtual void Close();
		void Dispose();
		virtual OSInt ExecuteNonQuery(Text::CString sql);
		virtual DB::DBReader *ExecuteReader(Text::CString sql);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		ConnError GetConnError();
		void SetEnableDebug(Bool enableDebug);
		void SetTraceFile(const WChar *fileName);
		UTF8Char *ShowTablesCmd(UTF8Char *sbuff);

		DBReader *GetTablesInfo(Text::CString schemaName);
		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names);
		virtual DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);

	public:
		void ShowSQLError(const UTF16Char *state, const UTF16Char *errMsg);
		void LogSQLError(void *hStmt);

		Text::String *GetConnStr();
		Text::String *GetConnDSN();
		Text::String *GetConnUID();
		Text::String *GetConnPWD();
		Text::String *GetConnSchema();

		static UOSInt GetDriverList(Data::ArrayList<Text::String*> *driverList);
		static IO::ConfigFile *GetDriverInfo(Text::CString driverName);
		static DBTool *CreateDBTool(Text::String *dsn, Text::String *uid, Text::String *pwd, Text::String *schema, IO::LogTool *log, Text::CString logPrefix);
		static DBTool *CreateDBTool(Text::CString dsn, Text::CString uid, Text::CString pwd, Text::CString schema, IO::LogTool *log, Text::CString logPrefix);
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
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb);
		virtual Text::String *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Geometry::Vector2D *GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);
		virtual Bool GetVariItem(UOSInt colIndex, Data::VariItem *item);

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);

		DB::DBUtil::ColType ODBCType2DBType(Int16 odbcType, UOSInt colSize);
	};
}
#endif
