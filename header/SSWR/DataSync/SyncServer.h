#ifndef _SM_SSWR_DATASYNC_SYNCSERVER
#define _SM_SSWR_DATASYNC_SYNCSERVER
#include "Data/Int32Map.h"
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
			typedef struct
			{
				Int32 serverId;
				const UTF8Char *serverName;
				Net::TCPClient *cli;
				Bool isLocal;
				Sync::Mutex *mut;
			} ServerInfo;

			typedef void (__stdcall *DataHandler)(void *userObj, const UInt8 *data, UOSInt dataSize);
		private:
			Net::SocketFactory *sockf;
			IO::IProtocolHandler *protoHdlr;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			DataHandler dataHdlr;
			void *dataObj;

			Sync::RWMutex *svrMut;
			Data::Int32Map<ServerInfo*> *svrMap;

			Data::ArrayList<SyncClient*> *syncCliList;

			static void __stdcall OnClientConn(UInt32 *s, void *userObj);
			static void __stdcall OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
		public:
			SyncServer(Net::SocketFactory *sockf, IO::LogTool *log, UInt16 port, Int32 serverId, const UTF8Char *serverName, const UTF8Char *syncClients, DataHandler dataHdlr, void *dataObj);
			virtual ~SyncServer();

			Bool IsError();
			UOSInt GetServerList(Data::ArrayList<ServerInfo*> *svrList);
			void SendUserData(const UInt8 *dataBuff, UOSInt dataSize);

			virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
			virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);
		};
	}
}
#endif
