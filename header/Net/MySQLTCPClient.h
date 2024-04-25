#ifndef _SM_NET_MYSQLTCPCLIENT
#define _SM_NET_MYSQLTCPCLIENT
#include "AnyType.h"
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
		NN<Net::SocketFactory> sockf;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;
		Net::TCPClient *cli;
		Sync::Mutex cliMut;
		Bool recvRunning;
		Bool recvStarted;
		NN<Text::String> userName;
		NN<Text::String> password;
		Optional<Text::String> database;
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

		static UInt32 __stdcall RecvThread(AnyType userObj);
		void SetLastError(Text::CString errMsg);

		void SendExecuteStmt(UInt32 stmtId);
		void SendStmtClose(UInt32 stmtId);
	public:
		MySQLTCPClient(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NN<Text::String> userName, NN<Text::String> password, Optional<Text::String> database);
		MySQLTCPClient(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Text::CString userName, Text::CString password, Text::CString database);
		virtual ~MySQLTCPClient();

		virtual DB::SQLType GetSQLType() const;
		virtual Bool IsAxisAware() const;
		virtual DB::DBConn::ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual void Dispose();
		virtual OSInt ExecuteNonQuery(Text::CStringNN sql);
		virtual Optional<DB::DBReader> ExecuteReader(Text::CStringNN sql);
		Optional<DB::DBReader> ExecuteReaderText(Text::CStringNN sql);
		Optional<DB::DBReader> ExecuteReaderBinary(Text::CStringNN sql);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual Optional<DB::DBTransaction> BeginTransaction();
		virtual void Commit(NN<DB::DBTransaction> tran);
		virtual void Rollback(NN<DB::DBTransaction> tran);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		Bool ChangeSchema(const UTF8Char *schemaName);

		Bool IsError();

		Bool ServerInfoRecv();
		Text::String *GetServerVer();
		UInt32 GetConnId();
		UOSInt GetAuthPluginData(UInt8 *buff);
		UInt32 GetServerCap() const;
		UInt16 GetServerCS() const;

		NN<const Net::SocketUtil::AddressInfo> GetConnAddr() const;
		UInt16 GetConnPort() const;
		Optional<Text::String> GetConnDB() const;
		NN<Text::String> GetConnUID() const;
		NN<Text::String> GetConnPWD() const;

		static UInt16 GetDefaultPort();
		static DB::DBTool *CreateDBTool(NN<Net::SocketFactory> sockf, NN<Text::String> serverName, Optional<Text::String> dbName, NN<Text::String> uid, NN<Text::String> pwd, NN<IO::LogTool> log, Text::CString logPrefix);
		static DB::DBTool *CreateDBTool(NN<Net::SocketFactory> sockf, Text::CStringNN serverName, Text::CString dbName, Text::CString uid, Text::CString pwd, NN<IO::LogTool> log, Text::CString logPrefix);
	};
}
#endif
