#ifndef _SM_NET_SSHFORWARDER
#define _SM_NET_SSHFORWARDER
#include "Net/SSHConn.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"

namespace Net
{
	class SSHForwarder
	{
	private:
		NotNullPtr<Net::SSHConn> conn;
		NotNullPtr<Net::TCPServer> svr;
		Net::TCPClientMgr cliMgr;
		IO::LogTool log;
		NotNullPtr<Text::String> remoteHost;
		UInt16 remotePort;

		static void __stdcall OnClientEvent(NotNullPtr<TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(NotNullPtr<TCPClient> cli, void *userObj, void *cliData, const Data::ByteArrayR &buff);
		static void __stdcall OnClientTimeout(NotNullPtr<TCPClient> cli, void *userObj, void *cliData);
		static void __stdcall OnClientConn(Socket *s, void *userObj);
	public:
		SSHForwarder(NotNullPtr<Net::SSHConn> conn, UInt16 localPort, Text::CStringNN remoteHost, UInt16 remotePort);
		~SSHForwarder();
		
		Bool IsError() const;
		UInt16 GetListenPort() const;
		void DoEvents();
		Bool HasChannels() const;
	};
}
#endif
