#ifndef _SM_NET_SNMPCLIENT
#define _SM_NET_SNMPCLIENT
#include "Net/SNMPUtil.h"
#include "Net/UDPServer.h"
#include "Sync/Mutex.h"

namespace Net
{
	class SNMPClient
	{
	private:
		Net::UDPServer *svr;
		Int32 reqId;
		Sync::Mutex mut;
		Sync::Event respEvt;
		Bool hasResp;
		Net::SNMPUtil::ErrorStatus respStatus;
		Optional<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> respList;
		Sync::Mutex scanMut;
		Optional<Data::ArrayListNN<Net::SocketUtil::AddressInfo>> scanList;

		static void __stdcall OnSNMPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
	public:
		SNMPClient(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log);
		~SNMPClient();

		Bool IsError();

		Net::SNMPUtil::ErrorStatus V1GetRequest(NN<const Net::SocketUtil::AddressInfo> agentAddr, NN<Text::String> community, const UTF8Char *oidText, UOSInt oidTextLen, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList);
		Net::SNMPUtil::ErrorStatus V1GetRequestPDU(NN<const Net::SocketUtil::AddressInfo> agentAddr, NN<Text::String> community, const UInt8 *oid, UOSInt oidLen, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList);
		Net::SNMPUtil::ErrorStatus V1GetNextRequest(NN<const Net::SocketUtil::AddressInfo> agentAddr, NN<Text::String> community, const UTF8Char *oidText, UOSInt oidTextLen, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList);
		Net::SNMPUtil::ErrorStatus V1GetNextRequestPDU(NN<const Net::SocketUtil::AddressInfo> agentAddr, NN<Text::String> community, const UInt8 *oid, UOSInt oidLen, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList);
		Net::SNMPUtil::ErrorStatus V1Walk(NN<const Net::SocketUtil::AddressInfo> agentAddr, NN<Text::String> community, const UTF8Char *oidText, UOSInt oidTextLen, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList);
		UOSInt V1ScanGetRequest(NN<const Net::SocketUtil::AddressInfo> broadcastAddr, NN<Text::String> community, const UTF8Char *oidText, UOSInt oidTextLen, NN<Data::ArrayListNN<Net::SocketUtil::AddressInfo>> addrList, Data::Duration timeout, Bool scanIP);
	};
}
#endif
