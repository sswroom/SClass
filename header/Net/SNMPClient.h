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
		Data::ArrayList<Net::SNMPUtil::BindingItem *> *respList;
		Sync::Mutex scanMut;
		Data::ArrayList<Net::SocketUtil::AddressInfo *> *scanList;

		static void __stdcall OnSNMPPacket(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
	public:
		SNMPClient(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<IO::LogTool> log);
		~SNMPClient();

		Bool IsError();

		Net::SNMPUtil::ErrorStatus V1GetRequest(NotNullPtr<const Net::SocketUtil::AddressInfo> agentAddr, NotNullPtr<Text::String> community, const UTF8Char *oidText, UOSInt oidTextLen, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList);
		Net::SNMPUtil::ErrorStatus V1GetRequestPDU(NotNullPtr<const Net::SocketUtil::AddressInfo> agentAddr, NotNullPtr<Text::String> community, const UInt8 *oid, UOSInt oidLen, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList);
		Net::SNMPUtil::ErrorStatus V1GetNextRequest(NotNullPtr<const Net::SocketUtil::AddressInfo> agentAddr, NotNullPtr<Text::String> community, const UTF8Char *oidText, UOSInt oidTextLen, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList);
		Net::SNMPUtil::ErrorStatus V1GetNextRequestPDU(NotNullPtr<const Net::SocketUtil::AddressInfo> agentAddr, NotNullPtr<Text::String> community, const UInt8 *oid, UOSInt oidLen, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList);
		Net::SNMPUtil::ErrorStatus V1Walk(NotNullPtr<const Net::SocketUtil::AddressInfo> agentAddr, NotNullPtr<Text::String> community, const UTF8Char *oidText, UOSInt oidTextLen, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList);
		UOSInt V1ScanGetRequest(NotNullPtr<const Net::SocketUtil::AddressInfo> broadcastAddr, NotNullPtr<Text::String> community, const UTF8Char *oidText, UOSInt oidTextLen, Data::ArrayList<Net::SocketUtil::AddressInfo *> *addrList, Data::Duration timeout, Bool scanIP);
	};
}
#endif
