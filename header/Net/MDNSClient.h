#ifndef _SM_NET_MDNSCLIENT
#define _SM_NET_MDNSCLIENT
#include "Net/DNSClient.h"
#include "Net/SocketFactory.h"
#include "Net/UDPServer.h"

namespace Net
{
	class MDNSClient
	{
	public:
		typedef void (CALLBACKFUNC DNSAnswerHandler)(AnyType userData, NN<const Net::SocketUtil::AddressInfo> addr, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> answers);
	private:
		NN<Net::SocketFactory> sockf;
		NN<Net::UDPServer> udp;
		IO::LogTool log;
		DNSAnswerHandler hdlr;
		AnyType hdlrObj;

		static void __stdcall OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
	public:
		MDNSClient(NN<Net::SocketFactory> sockf, DNSAnswerHandler hdlr, AnyType hdlrObj);
		~MDNSClient();

		Bool IsError();
		Bool SendQuery(Text::CStringNN domain);

		static UIntOS ParseAnswers(UnsafeArray<const UInt8> buff, UIntOS dataSize, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> answers);
	};
}
#endif
