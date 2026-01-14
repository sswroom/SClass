#ifndef _SM_NET_SOCKETUTIL
#define _SM_NET_SOCKETUTIL
#include "Text/CString.h"
#include "Text/PString.h"

namespace Net
{
	enum class AddrType
	{
		Unknown,
		IPv4,
		IPv6
	};

	enum class IPType
	{
		Public,
		Network,
		Local,
		Private,
		Broadcast,
		Multicast
	};

	class SocketUtil
	{
	public:
		typedef struct
		{
			AddrType addrType;
			UInt8 addr[20];
		} AddressInfo;
	public:
		static UnsafeArrayOpt<UTF8Char> GetAddrName(UnsafeArray<UTF8Char> buff, NN<const AddressInfo> addr);
		static UnsafeArrayOpt<WChar> GetAddrName(UnsafeArray<WChar> buff, NN<const AddressInfo> addr);
		static UnsafeArrayOpt<UTF8Char> GetAddrName(UnsafeArray<UTF8Char> buff, NN<const AddressInfo> addr, UInt16 port);
		static UnsafeArrayOpt<WChar> GetAddrName(UnsafeArray<WChar> buff, NN<const AddressInfo> addr, UInt16 port);
		static UnsafeArray<UTF8Char> GetIPv4Name(UnsafeArray<UTF8Char> buff, UInt32 ip);
		static UnsafeArray<WChar> GetIPv4Name(UnsafeArray<WChar> buff, UInt32 ip);
		static UnsafeArray<UTF8Char> GetIPv4Name(UnsafeArray<UTF8Char> buff, UInt32 ip, UInt16 port);
		static UnsafeArray<WChar> GetIPv4Name(UnsafeArray<WChar> buff, UInt32 ip, UInt16 port);
		static UInt32 GetIPAddr(UnsafeArray<const WChar> ipName);
		static UInt32 GetIPAddr(Text::CStringNN ipName);
		static UInt32 GetDefNetMaskv4(UInt32 ip);
		static void SetAddrInfoV4(NN<AddressInfo> addr, UInt32 ipv4);
		static void SetAddrInfoV6(NN<AddressInfo> addr, UnsafeArray<const UInt8> ipv6, Int32 zid);
		static Bool SetAddrInfo(NN<AddressInfo> addr, Text::CStringNN ipName);
		static void SetAddrInfoAnyV6(NN<AddressInfo> addr);
		static UInt32 CalcCliId(NN<const AddressInfo> addr);
		static UInt32 IPv4ToSortable(UInt32 ipv4);
		static Bool IPv4SubnetValid(UInt32 subnet);
		static IPType GetIPv4Type(UInt32 ipv4);
		static IPType GetIPType(NN<const AddressInfo> addr);
		static UInt32 IPv4ToBroadcast(UInt32 ipv4);
		static Bool AddrEquals(NN<const AddressInfo> addr1, NN<const AddressInfo> addr2);
		static Text::PString GetHostPort(Text::PString str, OutParam<UInt16> port);
	};
}
#endif
