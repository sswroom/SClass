#ifndef _SM_NET_DHCPSERVER
#define _SM_NET_DHCPSERVER
#include "Data/ArrayList.h"
#include "Data/UInt64Map.h"
#include "Net/UDPServer.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"

namespace Net
{
	class DHCPServer
	{
	public:
		typedef struct
		{
			UInt64 hwAddr;
			UInt32 assignedIP;
			Int64 assignTime;
			Bool updated;
			Text::String *hostName;
			Text::String *vendorClass;
		} DeviceStatus;
		
	private:
		Net::SocketFactory *sockf;
		Net::UDPServer *svr;
		UInt32 infIndex;
		UInt32 infIP;
		UInt32 subnet;
		UInt32 firstIP;
		UInt32 devCount;
		UInt32 gateway;
		Data::ArrayList<UInt32> dnsList;
		UInt32 ipLeaseTime;

		Sync::Mutex devMut;
		UInt8 *devUsed;
		Data::UInt64Map<DeviceStatus*> devMap;

		static void __stdcall PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
	public:
		DHCPServer(Net::SocketFactory *sockf, UInt32 infIP, UInt32 subnet, UInt32 firstIP, UInt32 devCount, UInt32 gateway, Data::ArrayList<UInt32> *dnsList);
		~DHCPServer();

		Bool IsError();

		void UseStatus(Sync::MutexUsage *mutUsage);
		Data::ArrayList<DeviceStatus*> *StatusGetList();
		UInt32 GetIPLeaseTime();
	};
}
#endif
