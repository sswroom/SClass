#ifndef _SM_NET_SNMPTRAPMONITOR
#define _SM_NET_SNMPTRAPMONITOR
#include "Net/SNMPUtil.h"
#include "Net/UDPServer.h"

namespace Net
{
	class SNMPTrapMonitor
	{
	public:
		typedef Bool (__stdcall *SNMPTrapHandler)(AnyType userObj, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NN<const Net::SNMPUtil::TrapInfo> trap, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList); // return true = no need to release
	private:
		Net::UDPServer *svr;
		SNMPTrapHandler hdlr;
		AnyType hdlrObj;

		static void __stdcall OnSNMPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
	public:
		SNMPTrapMonitor(NN<Net::SocketFactory> sockf, SNMPTrapHandler hdlr, AnyType userObj, NN<IO::LogTool> log);
		~SNMPTrapMonitor();

		Bool IsError();
	};
}
#endif
