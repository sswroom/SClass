#ifndef _SM_NET_NTPCLIENT
#define _SM_NET_NTPCLIENT
#include "Net/UDPServer.h"

namespace Net
{
	class NTPClient
	{
	private:
		Net::UDPServer *svr;
		Net::SocketFactory *sockf;
		Sync::Mutex mut;
		Sync::Event evt;

		Data::DateTime *resultTime;
		Bool hasResult;

		static void __stdcall PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
	public:
		NTPClient(Net::SocketFactory *sockf, UInt16 port);
		~NTPClient();

		Bool GetServerTime(Text::CString host, UInt16 port, Data::DateTime *svrTime); //def = 123
		Bool GetServerTime(const Net::SocketUtil::AddressInfo *addr, UInt16 port, Data::DateTime *svrTime); //def = 123

		static UInt16 GetDefaultPort();
	};
}
#endif
