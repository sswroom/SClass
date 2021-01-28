#ifndef _SM_NET_DNSSERVER
#define _SM_NET_DNSSERVER
#include "IO/ConsoleWriter.h"
#include "Net/UDPServer.h"

namespace Net
{
	class DNSServer
	{
	public:
		typedef void (__stdcall *DNSRequest)(void *userObj, const UTF8Char *reqName, Int32 reqType, Int32 reqClass, const Net::SocketUtil::AddressInfo *reqAddr, UInt16 reqPort, Int32 reqId);
	private:
		Net::UDPServer *svr;
		Net::SocketFactory *sockf;
		DNSRequest reqHdlr;
		void *reqObj;

		static void __stdcall PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		void InitServer(Net::SocketFactory *sockf, UInt16 port);
	public:
		DNSServer(Net::SocketFactory *sockf, UInt16 port);
		DNSServer(Net::SocketFactory *sockf);
		~DNSServer();

		Bool IsError();
		void HandleRequest(DNSRequest hdlr, void *userObj);

		void ReplyRequest(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, OSInt dataSize);
	};
};
#endif
