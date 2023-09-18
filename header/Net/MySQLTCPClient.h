#ifndef _SM_NET_MYSQLTCPCLIENT
#define _SM_NET_MYSQLTCPCLIENT
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "Net/MySQLUtil.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"

namespace Net
{
	class MySQLTCPReader;
	class MySQLTCPBinaryReader;
	class MySQLTCPClient : public DB::DBConn
	{
	private:
		enum class CmdResultType
		{
			Processing,
			ResultReady,
			Error,
			ResultEnd,
			ProcessingBinary,
			BinaryExecuting,
			BinaryResultReady
		};

		enum class ClientMode
		{
			Handshake,
			Authen,
			Data
		};
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;
		Net::TCPClient *cli;
		Sync::Mutex cliMut;
		Bool recvRunning;
		Bool recvStarted;
		NotNullPtr<Text::String> userName;
		NotNullPtr<Text::String> password;
		Text::String *database;
		ClientMode mode;
		Text::String *svrVer;
		Bool axisAware;
		UInt32 connId;
		UInt8 authPluginData[20];
		UOSInt authPluginDataSize;
		Net::MySQLUtil::AuthenType authenType;
		UInt32 svrCap;
		UInt16 svrCS;
		UInt16 connStatus;
		Text::String *lastError;

		Sync::Mutex cmdMut;
		Sync::Event cmdEvt;
		UOSInt cmdSeqNum;
		MySQLTCPReader *cmdTCPReader;
		MySQLTCPBinaryReader *cmdBinReader;
		CmdResultType cmdResultType;

		static UInt32 __stdcall RecvThread(void *userObj);
		void SetLastError(Text::CString errMsg);

		void SendExecuteStmt(UInt32 stmtId);
		void SendStmtClose(UInt32 stmtId);
	public:
		MySQLTCPClient(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NotNullPtr<Text::String> userName, NotNullPtr<Text::String> password, Text::String *database);
		MySQLTCPClient(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Text::CString userName, Text::CString password, Text::CString database);
		virtual ~MySQLTCPClient();

		virtual DB::SQLType GetSQLType() const;
		virtual Bool IsAxisAware() const;
		virtual DB::DBConn::ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual void Dispose();
		virtual OSInt ExecuteNonQuery(Text::CString sql);
		virtual DB::DBReader *ExecuteReader(Text::CString sql);
		DB::DBReader *ExecuteReaderText(Text::CString sql);
		DB::DBReader *ExecuteReaderBinary(Text::CString sql);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual void *BeginTransaction();
		virtual void Commit(void *tran);
		virtual void Rollback(void *tran);

		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names);
		virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		Bool ChangeSchema(const UTF8Char *schemaName);

		Bool IsError();

		Bool ServerInfoRecv();
		Text::String *GetServerVer();
		UInt32 GetConnId();
		UOSInt GetAuthPluginData(UInt8 *buff);
		UInt32 GetServerCap() const;
		UInt16 GetServerCS() const;

		NotNullPtr<const Net::SocketUtil::AddressInfo> GetConnAddr() const;
		UInt16 GetConnPort() const;
		Text::String *GetConnDB() const;
		NotNullPtr<Text::String> GetConnUID() const;
		NotNullPtr<Text::String> GetConnPWD() const;

		static UInt16 GetDefaultPort();
		static DB::DBTool *CreateDBTool(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<Text::String> serverName, Text::String *dbName, NotNullPtr<Text::String> uid, NotNullPtr<Text::String> pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);
		static DB::DBTool *CreateDBTool(NotNullPtr<Net::SocketFactory> sockf, Text::CStringNN serverName, Text::CString dbName, Text::CString uid, Text::CString pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix);
	};
}
#endif
