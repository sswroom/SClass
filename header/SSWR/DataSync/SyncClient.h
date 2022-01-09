#ifndef _SM_SSWR_DATASYNC_SYNCCLIENT
#define _SM_SSWR_DATASYNC_SYNCCLIENT
#include "Data/DateTime.h"
#include "IO/IProtocolHandler.h"
#include "Net/TCPClient.h"
#include "SSWR/DataSync/SyncClientDataMgr.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace SSWR
{
	namespace DataSync
	{
		class SyncClient : public IO::IProtocolHandler::DataListener
		{
		private:
			Net::SocketFactory *sockf;
			IO::IProtocolHandler *protoHdlr;
			Int32 serverId;
			Text::String *serverName;
			Sync::Mutex *cliMut;
			Net::TCPClient *cli;
			Text::String *cliHost;
			UInt16 cliPort;
			Data::DateTime *cliKATime;

			Sync::Event *recvEvt;
			Sync::Event *kaEvt;
			Bool recvRunning;
			Bool kaRunning;
			Bool toStop;

			SSWR::DataSync::SyncClientDataMgr *dataMgr;

			static UInt32 __stdcall RecvThread(void *userObj);
			static UInt32 __stdcall KAThread(void *userObj);

			Bool SendLogin();
			Bool SendKA();
			Bool SendUserData(const UInt8 *data, UOSInt dataSize);
		public:
			SyncClient(Net::SocketFactory *sockf, Int32 serverId, const UTF8Char *serverName, const UTF8Char *clientHost, UInt16 cliPort);
			virtual ~SyncClient();

			virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize);
			virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize);

			void AddUserData(const UInt8 *data, UOSInt dataSize);
		};
	}
}
#endif
