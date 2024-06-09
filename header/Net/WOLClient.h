#ifndef _SM_NET_WOLCLIENT
#define _SM_NET_WOLCLIENT
#include "Net/UDPServer.h"

namespace Net
{
	class WOLClient
	{
	private:
		Net::UDPServer *svr;
		NN<Net::SocketFactory> sockf;
		UInt32 adapterIP;

		static void __stdcall PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
	public:
		WOLClient(NN<Net::SocketFactory> sockf, UInt32 adapterIP, NN<IO::LogTool> log);
		~WOLClient();

		Bool IsError();
		Bool WakeDevice(const UInt8 *macAddr);
	};
}
#endif
