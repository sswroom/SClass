#ifndef _SM_NET_DHCPSERVER
#define _SM_NET_DHCPSERVER
#include "Data/ArrayList.h"
#include "Data/FastMap.h"
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
		NotNullPtr<Net::SocketFactory> sockf;
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
		Data::FastMap<UInt64, DeviceStatus*> devMap;

		static void __stdcall PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
	public:
		DHCPServer(NotNullPtr<Net::SocketFactory> sockf, UInt32 infIP, UInt32 subnet, UInt32 firstIP, UInt32 devCount, UInt32 gateway, NotNullPtr<Data::ArrayList<UInt32>> dnsList);
		~DHCPServer();

		Bool IsError() const;

		void UseStatus(Sync::MutexUsage *mutUsage) const;
		const Data::ReadingList<DeviceStatus*> *StatusGetList() const;
		UInt32 GetIPLeaseTime() const;
	};
}
#endif
