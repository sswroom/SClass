#ifndef _SM_NET_CONNECTIONINFO
#define _SM_NET_CONNECTIONINFO
#include "Data/ArrayList.hpp"
#include "Data/ArrayListUInt32.h"
#include "Data/Timestamp.h"
#include "Net/SocketUtil.h"
#include "Text/CString.h"

namespace Net
{
	class ConnectionInfo
	{
	public:
		enum class ConnectionType
		{
			Unknown,
			Ethernet,
			WiFi,
			Loopback,
			DialUp,
			Cellular,
			Bridge
		};

		typedef enum //RFC 2863
		{
			CS_UP = 1,
			CS_DOWN = 2,
			CS_TESTING = 3,
			CS_UNKNOWN = 4,
			CS_DORMANT = 5,
			CS_NOTPRESENT = 6,
			CS_LOWERLAYERDOWN = 7
		} ConnectionStatus;

		class ConnectionEntry
		{
		public:
			UOSInt index;
			UnsafeArrayOpt<const Char> internalName;
			UnsafeArrayOpt<const UTF8Char> name;
			UnsafeArrayOpt<const UTF8Char> description;
			UnsafeArrayOpt<const UTF8Char> dnsSuffix;
			Data::ArrayListUInt32 ipaddr;
			Data::ArrayListUInt32 dnsaddr;
			UInt32 defGW;
			UInt32 dhcpSvr;
			Data::Timestamp dhcpLeaseTime;
			Data::Timestamp dhcpLeaseExpire;
			UInt8 *physicalAddr;
			UOSInt physicalAddrLeng;
			UInt32 mtu;
			Bool dhcpEnabled;
			ConnectionType connType;
			ConnectionStatus connStatus;

			ConnectionEntry() : ipaddr(4), dnsaddr(4) {}
		};
	private:
		ConnectionEntry ent;

	public:
		ConnectionInfo(const ConnectionEntry *ent);
		ConnectionInfo(void *info);
		~ConnectionInfo();

		Bool SetInfo(void *info);

		UnsafeArrayOpt<UTF8Char> GetName(UnsafeArray<UTF8Char> buff);
		UnsafeArrayOpt<UTF8Char> GetDescription(UnsafeArray<UTF8Char> buff);
		UnsafeArrayOpt<UTF8Char> GetDNSSuffix(UnsafeArray<UTF8Char> buff);

		UInt32 GetIPAddress(UOSInt index);
		UInt32 GetDNSAddress(UOSInt index);
		UInt32 GetDefaultGW();
		UOSInt GetIndex();
		UOSInt GetPhysicalAddress(UInt8 *buff, UOSInt buffSize);
		UInt32 GetMTU();
		Bool IsDhcpEnabled();
		UInt32 GetDhcpServer();
		Data::Timestamp GetDhcpLeaseTime();
		Data::Timestamp GetDhcpLeaseExpire();
		ConnectionType GetConnectionType();
		ConnectionStatus GetConnectionStatus();

		static Text::CStringNN ConnectionTypeGetName(ConnectionType connType);
		static Text::CStringNN ConnectionStatusGetName(ConnectionStatus connStatus);
	};
}
#endif
