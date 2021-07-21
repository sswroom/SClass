#ifndef _SM_NET_CONNECTIONINFO
#define _SM_NET_CONNECTIONINFO
#include "Data/ArrayList.h"
#include "Data/ArrayListUInt32.h"
#include "Data/DateTime.h"
#include "Net/SocketUtil.h"

namespace Net
{
	class ConnectionInfo
	{
	public:
		typedef enum
		{
			CT_UNKNOWN,
			CT_ETHERNET,
			CT_WIFI,
			CT_LOOPBACK,
			CT_DIALUP
		} ConnectionType;

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

		typedef struct
		{
			UOSInt index;
			const Char *internalName;
			const UTF8Char *name;
			const UTF8Char *description;
			const UTF8Char *dnsSuffix;
			Data::ArrayListUInt32 *ipaddr;
			Data::ArrayListUInt32 *dnsaddr;
			UInt32 defGW;
			UInt32 dhcpSvr;
			Data::DateTime *dhcpLeaseTime;
			Data::DateTime *dhcpLeaseExpire;
			UInt8 *physicalAddr;
			UOSInt physicalAddrLeng;
			UInt32 mtu;
			Bool dhcpEnabled;
			ConnectionType connType;
			ConnectionStatus connStatus;
		} ConnectionEntry;
	private:
		ConnectionEntry ent;

	public:
		ConnectionInfo(const ConnectionEntry *ent);
		ConnectionInfo(void *info);
		~ConnectionInfo();

		Bool SetInfo(void *info);

		UTF8Char *GetName(UTF8Char *buff);
		UTF8Char *GetDescription(UTF8Char *buff);
		UTF8Char *GetDNSSuffix(UTF8Char *buff);

		UInt32 GetIPAddress(UOSInt index);
		UInt32 GetDNSAddress(UOSInt index);
		UInt32 GetDefaultGW();
		UOSInt GetIndex();
		UOSInt GetPhysicalAddress(UInt8 *buff, UOSInt buffSize);
		UInt32 GetMTU();
		Bool IsDhcpEnabled();
		UInt32 GetDhcpServer();
		Data::DateTime *GetDhcpLeaseTime();
		Data::DateTime *GetDhcpLeaseExpire();
		ConnectionType GetConnectionType();
		ConnectionStatus GetConnectionStatus();

		static const UTF8Char *GetConnectionTypeName(ConnectionType connType);
		static const UTF8Char *GetConnectionStatusName(ConnectionStatus connStatus);
	};
}
#endif
