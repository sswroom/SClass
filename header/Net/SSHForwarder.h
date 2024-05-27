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
		NN<Net::SSHConn> conn;
		NN<Net::TCPServer> svr;
		Net::TCPClientMgr cliMgr;
		IO::LogTool log;
		NN<Text::String> remoteHost;
		UInt16 remotePort;

		static void __stdcall OnClientEvent(NN<TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(NN<TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		static void __stdcall OnClientTimeout(NN<TCPClient> cli, AnyType userObj, AnyType cliData);
		static void __stdcall OnClientConn(NN<Socket> s, AnyType userObj);
	public:
		SSHForwarder(NN<Net::SSHConn> conn, UInt16 localPort, Text::CStringNN remoteHost, UInt16 remotePort);
		~SSHForwarder();
		
		Bool IsError() const;
		UInt16 GetListenPort() const;
		void DoEvents();
		Bool HasChannels() const;
	};
}
#endif
