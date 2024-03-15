#ifndef _SM_SSWR_DATASYNC_SYNCSERVER
#define _SM_SSWR_DATASYNC_SYNCSERVER
#include "Data/FastMap.h"
#include "IO/IProtocolHandler.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "SSWR/DataSync/SyncClient.h"
#include "Sync/Mutex.h"
#include "Sync/RWMutex.h"

namespace SSWR
{
	namespace DataSync
	{
		class SyncServer : public IO::IProtocolHandler::DataListener
		{
		public:
			struct ServerInfo
			{
				Int32 serverId;
				const UTF8Char *serverName;
				Net::TCPClient *cli;
				Bool isLocal;
				Sync::Mutex mut;
			};

			typedef void (__stdcall *DataHandler)(AnyType userObj, const UInt8 *data, UOSInt dataSize);
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			IO::ProtoHdlr::ProtoSyncHandler protoHdlr;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			DataHandler dataHdlr;
			AnyType dataObj;

			Sync::RWMutex svrMut;
			Data::FastMap<Int32, ServerInfo*> svrMap;

			Data::ArrayList<SyncClient*> syncCliList;

			static void __stdcall OnClientConn(Socket *s, AnyType userObj);
			static void __stdcall OnClientEvent(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall OnClientTimeout(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
		public:
			SyncServer(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<IO::LogTool> log, UInt16 port, Int32 serverId, Text::CString serverName, Text::CString syncClients, DataHandler dataHdlr, AnyType dataObj, Bool autoStart, Data::Duration cliTimeout);
			virtual ~SyncServer();

			Bool Start();
			Bool IsError();
			UOSInt GetServerList(Data::ArrayList<ServerInfo*> *svrList);
			void SendUserData(const UInt8 *dataBuff, UOSInt dataSize);

			virtual void DataParsed(NotNullPtr<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
			virtual void DataSkipped(NotNullPtr<IO::Stream> stm, AnyType stmObj, const UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
