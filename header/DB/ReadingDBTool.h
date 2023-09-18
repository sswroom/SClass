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
			Text::String *status;
			Text::String *user;
			Text::String *clientHostName;
			Text::String *dbName;
			Text::String *cmd;
			Int32 timeUsed;
			Text::String *sql;
		};

		typedef Int32 (__stdcall * SQLFailedFunc)(Text::CString sqlCmd, TriggerType trigType);

	protected:
		DB::DBConn *db;
		NotNullPtr<IO::LogTool> log;
		DB::DBReader *lastReader;
		UInt32 readerCnt;
		UInt32 readerFail;
//		UTF8Char *connStr;
		UInt32 openFail;
		Bool needRelease;
		Text::String *currDBName;

		Text::String *logPrefix;

		Bool isWorking;
		Int32 workId;
		SQLFailedFunc trig;
		Text::StringBuilderUTF8 lastErrMsg;

		UInt32 dataCnt;

		SQLType sqlType;
		Bool axisAware;

	public:
		static ReadingDBTool *MongoDBSource(const UTF8Char *url, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);

	protected:
		void AddLogMsgC(const UTF8Char *msg, UOSInt msgLen, IO::LogHandler::LogLevel logLev);

		UOSInt SplitMySQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr);
		UOSInt SplitMSSQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr);
		UOSInt SplitUnkSQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr);
	public:
		ReadingDBTool(DB::DBConn *db, Bool needRelease, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);
		virtual ~ReadingDBTool();

		void SetFailTrigger(SQLFailedFunc trig);
		DB::DBReader *ExecuteReader(Text::CString sqlCmd);
		virtual void CloseReader(DB::DBReader *r);
		DB::SQLType GetSQLType() const;
		Bool IsAxisAware() const;
		Bool IsDataError(const UTF8Char *errCode);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> sb);
		DB::DBConn *GetDBConn();
		Int8 GetTzQhr() const;
		virtual void Reconnect();

		virtual Bool IsDBTool() const;
		
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

		virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *arr);
		virtual UOSInt QuerySchemaNames(Data::ArrayList<Text::String *> *arr);
		virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);

		virtual UOSInt GetDatabaseNames(Data::ArrayListNN<Text::String> *arr);
		virtual void ReleaseDatabaseNames(Data::ArrayListNN<Text::String> *arr);
		virtual Bool ChangeDatabase(Text::CString databaseName);
		virtual Text::String *GetCurrDBName();
		Bool GetDBCollation(Text::CString databaseName, Collation *collation);

		UOSInt GetVariables(Data::ArrayList<Data::TwinItem<Text::String*, Text::String*>> *vars);
		void FreeVariables(Data::ArrayList<Data::TwinItem<Text::String*, Text::String*>> *vars);

		UOSInt GetConnectionInfo(Data::ArrayList<ConnectionInfo *> *conns);
		void FreeConnectionInfo(Data::ArrayList<ConnectionInfo *> *conns);

		UOSInt SplitSQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr);

		virtual Bool CanModify();
	};
}
#endif
