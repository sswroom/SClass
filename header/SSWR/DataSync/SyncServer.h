#ifndef _SM_SSWR_DATASYNC_SYNCSERVER
#define _SM_SSWR_DATASYNC_SYNCSERVER
#include "Data/FastMapNN.hpp"
#include "IO/ProtocolHandler.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "SSWR/DataSync/SyncClient.h"
#include "Sync/Mutex.h"
#include "Sync/RWMutex.h"

namespace SSWR
{
	namespace DataSync
	{
		class SyncServer : public IO::ProtocolHandler::DataListener
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

			typedef void (CALLBACKFUNC DataHandler)(AnyType userObj, UnsafeArray<const UInt8> data, UIntOS dataSize);
		private:
			NN<Net::SocketFactory> sockf;
			IO::ProtoHdlr::ProtoSyncHandler protoHdlr;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			DataHandler dataHdlr;
			AnyType dataObj;

			Sync::RWMutex svrMut;
			Data::FastMapNN<Int32, ServerInfo> svrMap;

			Data::ArrayListNN<SyncClient> syncCliList;

			static void __stdcall OnClientConn(NN<Socket> s, AnyType userObj);
			static void __stdcall OnClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall OnClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
		public:
			SyncServer(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log, UInt16 port, Int32 serverId, Text::CStringNN serverName, Text::CString syncClients, DataHandler dataHdlr, AnyType dataObj, Bool autoStart, Data::Duration cliTimeout);
			virtual ~SyncServer();

			Bool Start();
			Bool IsError();
			UIntOS GetServerList(NN<Data::ArrayListNN<ServerInfo>> svrList);
			void SendUserData(const UInt8 *dataBuff, UIntOS dataSize);

			virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UIntOS cmdSize);
			virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UIntOS buffSize);
		};
	}
}
#endif
