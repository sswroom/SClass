#ifndef _SM_DB_READINGDBTOOL
#define _SM_DB_READINGDBTOOL
#include "Data/TwinItem.h"
#include "DB/DBConn.h"
#include "DB/SQLBuilder.h"
#include "DB/TableDef.h"
#include "IO/LogTool.h"
#include "Sync/Mutex.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class ReadingDBTool : public ReadingDB
	{
	public:
		typedef enum
		{
			ReaderTrigger = 0,
			NonQueryTrigger = 1
		} TriggerType;

		struct ConnectionInfo
		{
			Int32 id;
			Optional<Text::String> status;
			Optional<Text::String> user;
			Optional<Text::String> clientHostName;
			Optional<Text::String> dbName;
			Optional<Text::String> cmd;
			Int32 timeUsed;
			Optional<Text::String> sql;
		};

		typedef Int32 (__stdcall * SQLFailedFunc)(Text::CString sqlCmd, TriggerType trigType);

	protected:
		NN<DB::DBConn> db;
		NN<IO::LogTool> log;
		Optional<DB::DBReader> lastReader;
		UInt32 readerCnt;
		UInt32 readerFail;
//		UnsafeArray<UTF8Char> connStr;
		UInt32 openFail;
		Bool needRelease;
		Text::String *currDBName;

		Optional<Text::String> logPrefix;

		Bool isWorking;
		Int32 workId;
		SQLFailedFunc trig;
		Text::StringBuilderUTF8 lastErrMsg;

		UInt32 dataCnt;

		SQLType sqlType;
		Bool axisAware;

	public:
		static ReadingDBTool *MongoDBSource(UnsafeArray<const UTF8Char> url, NN<IO::LogTool> log, Text::CString logPrefix);

	protected:
		void AddLogMsgC(Text::CStringNN msg, IO::LogHandler::LogLevel logLev);

		UOSInt SplitMySQL(UnsafeArray<UnsafeArray<UTF8Char>> outStrs, UOSInt maxCnt, UnsafeArray<UTF8Char> oriStr);
		UOSInt SplitMSSQL(UnsafeArray<UnsafeArray<UTF8Char>> outStrs, UOSInt maxCnt, UnsafeArray<UTF8Char> oriStr);
		UOSInt SplitUnkSQL(UnsafeArray<UnsafeArray<UTF8Char>> outStrs, UOSInt maxCnt, UnsafeArray<UTF8Char> oriStr);
	public:
		ReadingDBTool(NN<DB::DBConn> db, Bool needRelease, NN<IO::LogTool> log, Text::CString logPrefix);
		virtual ~ReadingDBTool();

		void SetFailTrigger(SQLFailedFunc trig);
		Optional<DB::DBReader> ExecuteReader(Text::CStringNN sqlCmd);
		virtual void CloseReader(NN<DB::DBReader> r);
		DB::SQLType GetSQLType() const;
		Bool IsAxisAware() const;
		Bool IsDataError(UnsafeArrayOpt<const UTF8Char> errCode);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> sb);
		NN<DB::DBConn> GetDBConn();
		Int8 GetTzQhr() const;
		virtual void Reconnect();

		virtual Bool IsDBTool() const;
		
		UnsafeArray<UTF8Char> DBColUTF8(UnsafeArray<UTF8Char> sqlstr, UnsafeArray<const UTF8Char> colName);
		UnsafeArray<UTF8Char> DBColW(UnsafeArray<UTF8Char> sqlstr, const WChar *colName);
		UnsafeArray<UTF8Char> DBTrim(UnsafeArray<UTF8Char> sqlstr, Text::CStringNN val);
		UnsafeArray<UTF8Char> DBStrUTF8(UnsafeArray<UTF8Char> sqlstr, UnsafeArrayOpt<const UTF8Char> str);
		UnsafeArray<UTF8Char> DBStrW(UnsafeArray<UTF8Char> sqlstr, const WChar *str);
		UnsafeArray<UTF8Char> DBInt32(UnsafeArray<UTF8Char> sqlstr, Int32 val);
		UnsafeArray<UTF8Char> DBInt64(UnsafeArray<UTF8Char> sqlstr, Int64 val);
		UnsafeArray<UTF8Char> DBDateTime(UnsafeArray<UTF8Char> sqlstr, Data::DateTime *dat);
		UnsafeArray<UTF8Char> DBSng(UnsafeArray<UTF8Char> sqlstr, Single val);
		UnsafeArray<UTF8Char> DBDbl(UnsafeArray<UTF8Char> sqlstr, Double val);
		UnsafeArray<UTF8Char> DBBool(UnsafeArray<UTF8Char> sqlstr, Bool val);
		UnsafeArray<UTF8Char> DBIsNull(UnsafeArray<UTF8Char> sqlstr, UnsafeArray<const UTF8Char> colName);

		UInt32 GetDataCnt();

		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> arr);
		virtual UOSInt QuerySchemaNames(NN<Data::ArrayListStringNN> arr);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);

		virtual UOSInt GetDatabaseNames(NN<Data::ArrayListStringNN> arr);
		virtual void ReleaseDatabaseNames(NN<Data::ArrayListStringNN> arr);
		virtual Bool ChangeDatabase(Text::CStringNN databaseName);
		virtual Optional<Text::String> GetCurrDBName();
		Bool GetDBCollation(Text::CStringNN databaseName, NN<Collation> collation);

		UOSInt GetVariables(NN<Data::ArrayList<Data::TwinItem<Optional<Text::String>, Optional<Text::String>>>> vars);
		void FreeVariables(NN<Data::ArrayList<Data::TwinItem<Optional<Text::String>, Optional<Text::String>>>> vars);

		UOSInt GetConnectionInfo(NN<Data::ArrayListNN<ConnectionInfo>> conns);
		void FreeConnectionInfo(NN<Data::ArrayListNN<ConnectionInfo>> conns);

		UOSInt SplitSQL(UnsafeArray<UnsafeArray<UTF8Char>> outStrs, UOSInt maxCnt, UnsafeArray<UTF8Char> oriStr);

		virtual Bool CanModify();
	};
}
#endif
