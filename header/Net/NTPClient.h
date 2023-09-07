#ifndef _SM_NET_NTPCLIENT
#define _SM_NET_NTPCLIENT
#include "Data/Timestamp.h"
#include "Net/UDPServer.h"

namespace Net
{
	class NTPClient
	{
	private:
		Net::UDPServer *svr;
		NotNullPtr<Net::SocketFactory> sockf;
		Sync::Mutex mut;
		Sync::Event evt;

		Data::Timestamp resultTime;
		Bool hasResult;

		static void __stdcall PacketHdlr(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
	public:
		NTPClient(NotNullPtr<Net::SocketFactory> sockf, UInt16 port);
		~NTPClient();

		Bool GetServerTime(Text::CStringNN host, UInt16 port, NotNullPtr<Data::DateTime> svrTime); //def = 123
		Bool GetServerTime(Text::CStringNN host, UInt16 port, OutParam<Data::Timestamp> svrTime); //def = 123
		Bool GetServerTime(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NotNullPtr<Data::DateTime> svrTime); //def = 123
		Bool GetServerTime(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, OutParam<Data::Timestamp> svrTime); //def = 123

		static UInt16 GetDefaultPort();
	};
}
#endif
