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
			AnyType cliObj;
		} ClientData;

	public:
		class TCPServerHandler
		{
		public:
			virtual AnyType NewConn(NN<Net::TCPClient> cli) = 0;
			virtual void EndConn(NN<Net::TCPClient> cli, AnyType cliObj) = 0;
			virtual UOSInt ReceivedData(NN<Net::TCPClient> cli, AnyType cliObj, const Data::ByteArrayR &buff) = 0; //Return buff size unprocessed
		};

	private:
		Net::TCPClientMgr *cliMgr;
		Net::TCPServer *svr;
		NN<Net::SocketFactory> sockf;
		UOSInt maxBuffSize;
		TCPServerHandler *hdlr;

	private:
		static void __stdcall ConnHdlr(NN<Socket> s, AnyType userObj);
		static void __stdcall EventHdlr(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall DataHdlr(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		static void __stdcall TimeoutHdlr(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
	public:
		TCPServerController(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log, UInt16 port, Text::CString prefix, UOSInt maxBuffSize, TCPServerHandler *hdlr, UOSInt workerCnt, Int32 timeoutSec, Bool autoStart);
		~TCPServerController();

		Bool Start();
		Bool IsError();
		void UseGetCli(NN<Sync::MutexUsage> mutUsage);
		UOSInt GetCliCount();
		Optional<Net::TCPClient> GetClient(UOSInt index, OutParam<AnyType> cliObj);
	};
}
#endif
