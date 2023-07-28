#ifndef _SM_NET_SNMPTRAPMONITOR
#define _SM_NET_SNMPTRAPMONITOR
#include "Net/SNMPUtil.h"
#include "Net/UDPServer.h"

namespace Net
{
	class SNMPTrapMonitor
	{
	public:
		typedef Bool (__stdcall *SNMPTrapHandler)(void *userObj, const Net::SocketUtil::AddressInfo *addr, UInt16 port, NotNullPtr<const Net::SNMPUtil::TrapInfo> trap, NotNullPtr<Data::ArrayList<Net::SNMPUtil::BindingItem*>> itemList); // return true = no need to release
	private:
		Net::UDPServer *svr;
		SNMPTrapHandler hdlr;
		void *hdlrObj;

		static void __stdcall OnSNMPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
	public:
		SNMPTrapMonitor(NotNullPtr<Net::SocketFactory> sockf, SNMPTrapHandler hdlr, void *userObj);
		~SNMPTrapMonitor();

		Bool IsError();
	};
}
#endif
