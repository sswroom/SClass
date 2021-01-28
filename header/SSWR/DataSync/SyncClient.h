#ifndef _SM_SSWR_DATASYNC_SYNCPROGCLIENT
#define _SM_SSWR_DATASYNC_SYNCPROGCLIENT
#include "Data/DateTime.h"
#include "IO/IProtocolHandler.h"
#include "Net/TCPClient.h"
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
			const UTF8Char *serverName;
			Sync::Mutex *cliMut;
			Net::TCPClient *cli;
			const UTF8Char *cliHost;
			UInt16 cliPort;
			Data::DateTime *cliKATime;

			Sync::Event *recvEvt;
			Sync::Event *kaEvt;
			Bool recvRunning;
			Bool kaRunning;
			Bool toStop;

			Sync::Mutex *userDataMut;
			Data::ArrayList<UInt8*> *userDataList;

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
