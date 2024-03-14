#ifndef _SM_NET_DNSSERVER
#define _SM_NET_DNSSERVER
#include "AnyType.h"
#include "IO/ConsoleWriter.h"
#include "Net/UDPServer.h"

namespace Net
{
	class DNSServer
	{
	public:
		typedef void (__stdcall *DNSRequest)(AnyType userObj, Text::CString reqName, Int32 reqType, Int32 reqClass, NotNullPtr<const Net::SocketUtil::AddressInfo> reqAddr, UInt16 reqPort, UInt32 reqId);
	private:
		Net::UDPServer *svr;
		NotNullPtr<Net::SocketFactory> sockf;
		DNSRequest reqHdlr;
		AnyType reqObj;

		static void __stdcall PacketHdlr(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		void InitServer(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, NotNullPtr<IO::LogTool> log);
	public:
		DNSServer(NotNullPtr<Net::SocketFactory> sockf, UInt16 port, NotNullPtr<IO::LogTool> log);
		DNSServer(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<IO::LogTool> log);
		~DNSServer();

		Bool IsError();
		void HandleRequest(DNSRequest hdlr, AnyType userObj);

		void ReplyRequest(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize);
	};
}
#endif
