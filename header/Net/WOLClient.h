#ifndef _SM_NET_WOLCLIENT
#define _SM_NET_WOLCLIENT
#include "Net/UDPServer.h"

namespace Net
{
	class WOLClient
	{
	private:
		Net::UDPServer *svr;
		NotNullPtr<Net::SocketFactory> sockf;
		UInt32 adapterIP;

		static void __stdcall PacketHdlr(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
	public:
		WOLClient(NotNullPtr<Net::SocketFactory> sockf, UInt32 adapterIP, NotNullPtr<IO::LogTool> log);
		~WOLClient();

		Bool IsError();
		Bool WakeDevice(const UInt8 *macAddr);
	};
}
#endif
