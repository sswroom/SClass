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
		NotNullPtr<Net::SocketFactory> sockf;
		NotNullPtr<IO::LogTool> log;
		UInt16 port;
		Net::TCPServer *svr;
		Net::TCPClientMgr *cliMgr;

		static void __stdcall OnClientConn(Socket *s, AnyType userObj);
		static void __stdcall OnClientEvent(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		static void __stdcall OnClientTimeout(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData);

	public:
		ProxyServer(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, NotNullPtr<IO::LogTool> log, Bool autoStart);
		~ProxyServer();

		Bool Start();
		Bool IsError();
	};
}
#endif
