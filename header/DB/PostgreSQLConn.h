#ifndef _SM_DB_POSTGRESQLCONN
#define _SM_DB_POSTGRESQLCONN
#include "Data/ArrayList.h"
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
		ClassData *clsData;

		Bool isTran;
		Text::String *server;
		UInt16 port;
		Text::String *database;
		Text::String *uid;
		Text::String *pwd;
		IO::LogTool *log;
		Int8 tzQhr;

		Data::ArrayList<Text::String*> *tableNames;

		void Connect();
		void ClearTableNames();

	public:
		PostgreSQLConn(Text::String *server, UInt16 port, Text::String *uid, Text::String *pwd, Text::String *database, IO::LogTool *log);
		PostgreSQLConn(Text::CString server, UInt16 port, Text::CString uid, Text::CString pwd, Text::CString database, IO::LogTool *log);
		virtual ~PostgreSQLConn();
		virtual DB::DBUtil::ServerType GetSvrType();
		virtual ConnType GetConnType();
		virtual Int8 GetTzQhr();
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(Text::StringBuilderUTF8 *sb);
		virtual void Close();
		virtual void Dispose();
		virtual OSInt ExecuteNonQuery(Text::CString sql);
		virtual DBReader *ExecuteReader(Text::CString sql);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		virtual UOSInt GetTableNames(Data::ArrayList<Text::CString> *names);
		virtual DBReader *QueryTableData(Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);

		Bool IsConnError();
		Text::String *GetConnServer();
		Text::String *GetConnDB();
		Text::String *GetConnUID();
		Text::String *GetConnPWD();

		static Text::CString ExecStatusTypeGetName(OSInt status);
		static DB::DBUtil::ColType DBType2ColType(UInt32 dbType);
		static DBTool *CreateDBTool(Net::SocketFactory *sockf, Text::String *serverName, UInt16 port, Text::String *dbName, Text::String *uid, Text::String *pwd, IO::LogTool *log, Text::CString logPrefix);
		static DBTool *CreateDBTool(Net::SocketFactory *sockf, Text::CString serverName, UInt16 port, Text::CString dbName, Text::CString uid, Text::CString pwd, IO::LogTool *log, Text::CString logPrefix);
	};
}
#endif