#ifndef _SM_NET_WSDISCOVERYCLIENT
#define _SM_NET_WSDISCOVERYCLIENT
#include "Net/UDPServer.h"

namespace Net
{
	class WSDiscoveryClient
	{
	private:
		NN<Net::UDPServer> svr;
		NN<Net::SocketFactory> sockf;
		IO::LogTool log;

		static void __stdcall PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
	public:
		WSDiscoveryClient(NN<Net::SocketFactory> sockf);
		~WSDiscoveryClient();

		Bool IsError() const;
		Bool Probe(Text::CStringNN type, Text::CStringNN ns, Text::CStringNN nsName) const;
	};
}
#endif
