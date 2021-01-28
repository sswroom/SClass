#ifndef _SM_NET_SOCKETUTIL
#define _SM_NET_SOCKETUTIL
#include "Data/ArrayList.h"

namespace Net
{
	class SocketUtil
	{
	public:
		typedef enum
		{
			AT_UNKNOWN,
			AT_IPV4,
			AT_IPV6
		} AddrType;

		typedef enum
		{
			IT_PUBLIC,
			IT_NETWORK,
			IT_LOCAL,
			IT_PRIVATE,
			IT_BROADCAST,
			IT_MULTICAST
		} IPType;

		typedef struct
		{
			AddrType addrType;
			UInt8 addr[20];
		} AddressInfo;
	public:
		static UTF8Char *GetAddrName(UTF8Char *buff, const AddressInfo *addr);
		static WChar *GetAddrName(WChar *buff, const AddressInfo *addr);
		static UTF8Char *GetAddrName(UTF8Char *buff, const AddressInfo *addr, UInt16 port);
		static WChar *GetAddrName(WChar *buff, const AddressInfo *addr, UInt16 port);
		static UTF8Char *GetIPv4Name(UTF8Char *buff, UInt32 ip);
		static WChar *GetIPv4Name(WChar *buff, UInt32 ip);
		static UTF8Char *GetIPv4Name(UTF8Char *buff, UInt32 ip, UInt16 port);
		static WChar *GetIPv4Name(WChar *buff, UInt32 ip, UInt16 port);
		static Bool GetIPAddr(const WChar *ipName, AddressInfo *addr);
		static Bool GetIPAddr(const Char *ipName, AddressInfo *addr);
		static Bool GetIPAddr(const UTF8Char *ipName, AddressInfo *addr);
		static UInt32 GetIPAddr(const WChar *ipName);
		static UInt32 GetIPAddr(const Char *ipName);
		static UInt32 GetIPAddr(const UTF8Char *ipName);
		static UInt32 GetDefNetMaskv4(UInt32 ip);
		static void SetAddrInfoV4(AddressInfo *addr, UInt32 ipv4);
		static void SetAddrInfoV6(AddressInfo *addr, const UInt8 *ipv6, Int32 zid);
		static void SetAddrAnyV6(AddressInfo *addr);
		static UInt32 CalcCliId(const AddressInfo *addr);
		static UInt32 IPv4ToSortable(UInt32 ipv4);
		static Bool IPv4SubnetValid(UInt32 subnet);
		static IPType GetIPv4Type(UInt32 ipv4);
		static UInt32 IPv4ToBroadcast(UInt32 ipv4);
		static Bool AddrEquals(const AddressInfo *addr1, const AddressInfo *addr2);
	};
}
#endif
