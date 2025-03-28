#ifndef _SM_NET_MDNSCLIENT
#define _SM_NET_MDNSCLIENT
#include "Net/SocketFactory.h"
#include "Net/UDPServer.h"

namespace Net
{
	class MDNSClient
	{
	private:
		NN<Net::SocketFactory> sockf;
		NN<Net::UDPServer> udp;
		IO::LogTool log;

		static void __stdcall OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
	public:
		MDNSClient(NN<Net::SocketFactory> sockf);
		~MDNSClient();

		Bool IsError();
	};
}
#endif
