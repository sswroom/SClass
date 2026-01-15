#ifndef _SM_NET_LORAGWMONITOR
#define _SM_NET_LORAGWMONITOR
#include "Net/SocketMonitor.h"

namespace Net
{
	class LoRaGWMonitor
	{
	public:
		typedef void (CALLBACKFUNC GWMPMessage)(AnyType userObj, Bool toServer, UInt8 ver, UInt16 token, UInt8 msgType, UnsafeArray<const UInt8> msg, UIntOS msgSize);
	private:
		NN<Net::SocketFactory> sockf;
		Optional<Socket> s;
		UInt16 port;
		Optional<Net::SocketMonitor> socMon;
		GWMPMessage msgHdlr;
		AnyType msgHdlrObj;

		static void __stdcall OnRAWPacket(AnyType userData, UnsafeArray<const UInt8> packetData, UIntOS packetSize);
	public:
		LoRaGWMonitor(NN<Net::SocketFactory> sockf, UInt16 port, GWMPMessage msgHdlr, AnyType msgHdlrObj);
		~LoRaGWMonitor();

		Bool IsError();
	};
}
#endif