#ifndef _SM_NET_MYSQLTCPCLIENT
#define _SM_NET_MYSQLTCPCLIENT
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace Net
{
	class MySQLTCPClient : public DB::DBConn
	{
	private:
		Net::SocketFactory *sockf;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;
		Net::TCPClient *cli;
		Sync::Mutex *cliMut;
		Bool recvRunning;
		Bool recvStarted;
		Text::String *userName;
		Text::String *password;
		Text::String *database;
		Int32 mode;
		Text::String *svrVer;
		UInt32 connId;
		UInt8 authPluginData[20];
		UOSInt authPluginDataSize;
		UInt32 svrCap;
		UInt16 svrCS;
		UInt16 connStatus;
		Text::String *lastError;
		
		Data::ArrayList<const UTF8Char *> *tableNames;

		Sync::Mutex *cmdMut;
		Sync::Event *cmdEvt;
		UOSInt cmdSeqNum;
		DB::DBReader *cmdReader;
		OSInt cmdResultType; // 0 = processing, 1 = result ready, 2 = error, 3 = result end

		static UInt32 __stdcall RecvThread(void *userObj);
		void SetLastError(const UTF8Char *errMsg, UOSInt msgLen);
		void SetLastError(const UTF8Char *errMsg);
	public:
		MySQLTCPClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, Text::String *userName, Text::String *password, Text::String *database);
		MySQLTCPClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UTF8Char *userName, const UTF8Char *password, const UTF8Char *database);
		virtual ~MySQLTCPClient();

		virtual DB::DBUtil::ServerType GetSvrType();
		virtual DB::DBConn::ConnType GetConnType();
		virtual Int8 GetTzQhr();
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(Text::StringBuilderUTF *sb);
		virtual void Close();
		virtual void Dispose();
		virtual OSInt ExecuteNonQuery(const UTF8Char *sql);
		virtual DB::DBReader *ExecuteReader(const UTF8Char *sql);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF *str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names);
		virtual DB::DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition);
		Bool ChangeSchema(const UTF8Char *schemaName);

		Bool IsError();

		Bool ServerInfoRecv();
		Text::String *GetServerVer();
		UInt32 GetConnId();
		UOSInt GetAuthPluginData(UInt8 *buff);
		UInt32 GetServerCap();
		UInt16 GetServerCS();

		const Net::SocketUtil::AddressInfo *GetConnAddr();
		UInt16 GetConnPort();
		Text::String *GetConnDB();
		Text::String *GetConnUID();
		Text::String *GetConnPWD();

		static UInt16 GetDefaultPort();
		static DB::DBTool *CreateDBTool(Net::SocketFactory *sockf, Text::String *serverName, Text::String *dbName, Text::String *uid, Text::String *pwd, IO::LogTool *log, const UTF8Char *logPrefix);
		static DB::DBTool *CreateDBTool(Net::SocketFactory *sockf, const UTF8Char *serverName, const UTF8Char *dbName, const UTF8Char *uid, const UTF8Char *pwd, IO::LogTool *log, const UTF8Char *logPrefix);
	};
}
#endif
