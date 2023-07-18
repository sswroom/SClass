#ifndef _SM_NET_LORAGWMONITOR
#define _SM_NET_LORAGWMONITOR
#include "Net/SocketMonitor.h"

namespace Net
{
	class LoRaGWMonitor
	{
	public:
		typedef void (__stdcall *GWMPMessage)(void *userObj, Bool toServer, UInt8 ver, UInt16 token, UInt8 msgType, const UInt8 *msg, UOSInt msgSize);
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Socket *s;
		UInt16 port;
		Net::SocketMonitor *socMon;
		GWMPMessage msgHdlr;
		void *msgHdlrObj;

		static void __stdcall OnRAWPacket(void *userData, const UInt8 *packetData, UOSInt packetSize);
	public:
		LoRaGWMonitor(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, GWMPMessage msgHdlr, void *msgHdlrObj);
		~LoRaGWMonitor();

		Bool IsError();
	};
}
#endif