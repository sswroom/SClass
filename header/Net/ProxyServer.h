#ifndef _SM_NET_PROXYSERVER
#define _SM_NET_PROXYSERVER
#include "IO/LogTool.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "Sync/Event.h"

namespace Net
{
	class ProxyServer
	{
	private:
		Net::SocketFactory *sockf;
		IO::LogTool *log;
		UInt16 port;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;

		static void __stdcall OnClientConn(Socket *s, void *userObj);
		static void __stdcall OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		static void __stdcall OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);

	public:
		ProxyServer(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log, Bool autoStart);
		~ProxyServer();

		Bool Start();
		Bool IsError();
	};
}
#endif
