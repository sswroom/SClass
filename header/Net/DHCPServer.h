#ifndef _SM_NET_DHCPSERVER
#define _SM_NET_DHCPSERVER
#include "Data/ArrayList.h"
#include "Data/FastMapNN.h"
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
		NN<Net::SocketFactory> sockf;
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
		Data::FastMapNN<UInt64, DeviceStatus> devMap;

		static void __stdcall PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
	public:
		DHCPServer(NN<Net::SocketFactory> sockf, UInt32 infIP, UInt32 subnet, UInt32 firstIP, UInt32 devCount, UInt32 gateway, NN<Data::ArrayList<UInt32>> dnsList, NN<IO::LogTool> log);
		~DHCPServer();

		Bool IsError() const;

		void UseStatus(NN<Sync::MutexUsage> mutUsage) const;
		NN<const Data::ReadingListNN<DeviceStatus>> StatusGetList() const;
		UInt32 GetIPLeaseTime() const;
	};
}
#endif
