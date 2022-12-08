#ifndef _SM_NET_TCPSERVERCONTROLLER
#define _SM_NET_TCPSERVERCONTROLLER
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "Sync/Mutex.h"

namespace Net
{
	class TCPServerController
	{
	private:
		typedef struct
		{
			UInt8 *buff;
			UOSInt buffSize;
			void *cliObj;
		} ClientData;

	public:
		class TCPServerHandler
		{
		public:
			virtual void *NewConn(Net::TCPClient *cli) = 0;
			virtual void EndConn(Net::TCPClient *cli, void *cliObj) = 0;
			virtual UOSInt ReceivedData(Net::TCPClient *cli, void *cliObj, UInt8 *buff, UOSInt buffSize) = 0; //Return buff size unprocessed
		};

	private:
		Net::TCPClientMgr *cliMgr;
		Net::TCPServer *svr;
		Net::SocketFactory *sockf;
		UOSInt maxBuffSize;
		TCPServerHandler *hdlr;

	private:
		static void __stdcall ConnHdlr(Socket *s, void *userObj);
		static void __stdcall EventHdlr(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall DataHdlr(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		static void __stdcall TimeoutHdlr(Net::TCPClient *cli, void *userObj, void *cliData);
	public:
		TCPServerController(Net::SocketFactory *sockf, IO::LogTool *log, UInt16 port, Text::CString prefix, UOSInt maxBuffSize, TCPServerHandler *hdlr, UOSInt workerCnt, Int32 timeoutSec, Bool autoStart);
		~TCPServerController();

		Bool Start();
		Bool IsError();
		void UseGetCli(Sync::MutexUsage *mutUsage);
		UOSInt GetCliCount();
		Net::TCPClient *GetClient(UOSInt index, void **cliObj);
	};
}
#endif
