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

		Bool isTran;
		Text::String *server;
		Text::String *database;
		Text::String *uid;
		Text::String *pwd;
		IO::LogTool *log;

		Data::ArrayList<const UTF8Char *> *tableNames;

		void Connect();

	public:
		MySQLConn(const UTF8Char *server, const UTF8Char *uid, const UTF8Char *pwd, const UTF8Char *database, IO::LogTool *log);
		MySQLConn(const WChar *server, const WChar *uid, const WChar *pwd, const WChar *database, IO::LogTool *log);
		virtual ~MySQLConn();
		virtual DB::DBUtil::ServerType GetSvrType();
		virtual ConnType GetConnType();
		virtual Int8 GetTzQhr();
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(Text::StringBuilderUTF8 *sb);
		virtual void Close();
		virtual void Dispose();
		virtual OSInt ExecuteNonQuery(const UTF8Char *sql);
//		virtual OSInt ExecuteNonQuery(const WChar *sql);
		virtual DBReader *ExecuteReader(const UTF8Char *sql);
//		virtual DBReader *ExecuteReader(const WChar *sql);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names);
		virtual DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition);

		Bool IsConnError();
		Text::String *GetConnServer();
		Text::String *GetConnDB();
		Text::String *GetConnUID();
		Text::String *GetConnPWD();

		static DBTool *CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log);
		static DBTool *CreateDBTool(Net::SocketFactory *sockf, const UTF8Char *serverName, const UTF8Char *dbName, const UTF8Char *uid, const UTF8Char *pwd, IO::LogTool *log, Text::CString logPrefix);
		static DBTool *CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log, Text::CString logPrefix);
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
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb);
		virtual Text::String *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Vector2D *GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

//		virtual WChar *GetName(OSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
	
		DB::DBUtil::ColType ToColType(Int32 dbType, UInt32 flags, UOSInt colSize);
	};
}
#endif
