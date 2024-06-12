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
		typedef void (__stdcall *DNSRequest)(AnyType userObj, Text::CStringNN reqName, Int32 reqType, Int32 reqClass, NN<const Net::SocketUtil::AddressInfo> reqAddr, UInt16 reqPort, UInt32 reqId);
	private:
		Net::UDPServer *svr;
		NN<Net::SocketFactory> sockf;
		DNSRequest reqHdlr;
		AnyType reqObj;

		static void __stdcall PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
		void InitServer(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log);
	public:
		DNSServer(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log);
		DNSServer(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log);
		~DNSServer();

		Bool IsError();
		void HandleRequest(DNSRequest hdlr, AnyType userObj);

		void ReplyRequest(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize);
	};
}
#endif
