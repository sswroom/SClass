#ifndef _SM_DB_READINGDBTOOL
#define _SM_DB_READINGDBTOOL
#include "DB/DBConn.h"
#include "DB/SQLBuilder.h"
#include "DB/TableDef.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class ReadingDBTool
	{
	public:
		typedef enum
		{
			ReaderTrigger = 0,
			NonQueryTrigger = 1
		} TriggerType;

		typedef Int32 (__stdcall * SQLFailedFunc)(Text::CString sqlCmd, TriggerType trigType);

	protected:
		DB::DBConn *db;
		IO::LogTool *log;
		DB::DBReader *lastReader;
		UInt32 readerCnt;
		UInt32 readerFail;
//		UTF8Char *connStr;
		UInt32 openFail;
		Bool needRelease;

		Text::String *logPrefix;

		Bool isWorking;
		Int32 workId;
		SQLFailedFunc trig;
		Text::StringBuilderUTF8 lastErrMsg;

		UInt32 dataCnt;

		DBUtil::ServerType svrType;

	public:
		static ReadingDBTool *MongoDBSource(const UTF8Char *url, IO::LogTool *log, Text::CString logPrefix);

	protected:
		void AddLogMsgC(const UTF8Char *msg, UOSInt msgLen, IO::ILogHandler::LogLevel logLev);

		UOSInt SplitMySQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr);
		UOSInt SplitMSSQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr);
		UOSInt SplitUnkSQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr);
	public:
		ReadingDBTool(DB::DBConn *db, Bool needRelease, IO::LogTool *log, Text::CString logPrefix);
		virtual ~ReadingDBTool();

		void SetFailTrigger(SQLFailedFunc trig);
		DB::DBReader *ExecuteReader(Text::CString sqlCmd);
		void CloseReader(DB::DBReader *r);
		DB::DBUtil::ServerType GetSvrType();
		Bool IsDataError(const UTF8Char *errCode);
		void GetLastErrorMsg(Text::StringBuilderUTF8 *sb);
		DB::DBConn *GetDBConn();
		Int8 GetTzQhr();
		
		UTF8Char *DBColUTF8(UTF8Char *sqlstr, const UTF8Char *colName);
		UTF8Char *DBColW(UTF8Char *sqlstr, const WChar *colName);
		UTF8Char *DBTrim(UTF8Char *sqlstr, Text::CString val);
		UTF8Char *DBStrUTF8(UTF8Char *sqlstr, const UTF8Char *str);
		UTF8Char *DBStrW(UTF8Char *sqlstr, const WChar *str);
		UTF8Char *DBInt32(UTF8Char *sqlstr, Int32 val);
		UTF8Char *DBInt64(UTF8Char *sqlstr, Int64 val);
		UTF8Char *DBDate(UTF8Char *sqlstr, Data::DateTime *dat);
		UTF8Char *DBSng(UTF8Char *sqlstr, Single val);
		UTF8Char *DBDbl(UTF8Char *sqlstr, Double val);
		UTF8Char *DBBool(UTF8Char *sqlstr, Bool val);
		UTF8Char *DBIsNull(UTF8Char *sqlstr, const UTF8Char *colName);

		UInt32 GetDataCnt();

		DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String *> *arr);
		UOSInt QuerySchemaNames(Data::ArrayList<Text::String *> *arr);
		DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);

		UOSInt GetDatabaseNames(Data::ArrayList<const UTF8Char*> *arr);
		void ReleaseDatabaseNames(Data::ArrayList<const UTF8Char*> *arr);
		Bool ChangeDatabase(const UTF8Char *databaseName);

		UOSInt SplitSQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr);

	protected:
		static void AppendColDef(DB::DBUtil::ServerType svrType, DB::SQLBuilder *sql, DB::ColDef *col);
		static void AppendColType(DB::DBUtil::ServerType svrType, DB::SQLBuilder *sql, DB::DBUtil::ColType colType, UOSInt colSize, Bool autoInc);
	};
}
#endif
