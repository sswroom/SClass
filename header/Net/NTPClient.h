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
		NN<Net::SocketFactory> sockf;
		Sync::Mutex mut;
		Sync::Event evt;

		Data::Timestamp resultTime;
		Bool hasResult;

		static void __stdcall PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
	public:
		NTPClient(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log);
		~NTPClient();

		Bool GetServerTime(Text::CStringNN host, UInt16 port, NN<Data::DateTime> svrTime); //def = 123
		Bool GetServerTime(Text::CStringNN host, UInt16 port, OutParam<Data::Timestamp> svrTime); //def = 123
		Bool GetServerTime(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NN<Data::DateTime> svrTime); //def = 123
		Bool GetServerTime(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, OutParam<Data::Timestamp> svrTime); //def = 123

		static UInt16 GetDefaultPort();
	};
}
#endif
