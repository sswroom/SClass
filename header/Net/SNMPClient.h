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
		Sync::Mutex *mut;
		Sync::Event *respEvt;
		Bool hasResp;
		Net::SNMPUtil::ErrorStatus respStatus;
		Data::ArrayList<Net::SNMPUtil::BindingItem *> *respList;
		Sync::Mutex *scanMut;
		Data::ArrayList<Net::SocketUtil::AddressInfo *> *scanList;

		static void __stdcall OnSNMPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
	public:
		SNMPClient(Net::SocketFactory *sockf);
		~SNMPClient();

		Bool IsError();

		Net::SNMPUtil::ErrorStatus V1GetRequest(const Net::SocketUtil::AddressInfo *agentAddr, const UTF8Char *community, const UTF8Char *oid, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList);
		Net::SNMPUtil::ErrorStatus V1GetRequestPDU(const Net::SocketUtil::AddressInfo *agentAddr, const UTF8Char *community, const UInt8 *oid, UOSInt oidLen, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList);
		Net::SNMPUtil::ErrorStatus V1GetNextRequest(const Net::SocketUtil::AddressInfo *agentAddr, const UTF8Char *community, const UTF8Char *oid, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList);
		Net::SNMPUtil::ErrorStatus V1GetNextRequestPDU(const Net::SocketUtil::AddressInfo *agentAddr, const UTF8Char *community, const UInt8 *oid, UOSInt oidLen, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList);
		Net::SNMPUtil::ErrorStatus V1Walk(const Net::SocketUtil::AddressInfo *agentAddr, const UTF8Char *community, const UTF8Char *oid, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList);
		OSInt V1ScanGetRequest(const Net::SocketUtil::AddressInfo *broadcastAddr, const UTF8Char *community, const UTF8Char *oid, Data::ArrayList<Net::SocketUtil::AddressInfo *> *addrList, UOSInt timeoutMS, Bool scanIP);
	};
}
#endif
