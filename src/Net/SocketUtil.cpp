#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "Net/SocketUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

UTF8Char *Net::SocketUtil::GetAddrName(UTF8Char *buff, const Net::SocketUtil::AddressInfo *addr)
{
	if (addr->addrType == AddrType::IPv4)
	{
		UTF8Char *ptr;
		ptr = Text::StrInt32(buff, addr->addr[0]);
		*ptr++ = '.';
		ptr = Text::StrInt32(ptr, addr->addr[1]);
		*ptr++ = '.';
		ptr = Text::StrInt32(ptr, addr->addr[2]);
		*ptr++ = '.';
		ptr = Text::StrInt32(ptr, addr->addr[3]);
		return ptr;
	}
	else if (addr->addrType == AddrType::IPv6)
	{
		UTF8Char *oriBuff = buff;
		Bool skipFound = false;
		Bool lastIsSkip = false;
		OSInt i = 0;
		UInt16 v;
		while (i < 16)
		{
			v = ReadMUInt16(&addr->addr[i]);
			if (v == 0)
			{
				if (!lastIsSkip)
				{
					if (skipFound)
					{
						*buff++ = ':';
						buff = Text::StrHexVal32V(buff, v);
					}
					else
					{
						lastIsSkip = true;
						skipFound = true;
						*buff++ = ':';
					}
				}
			}
			else
			{
				lastIsSkip = false;
				if (i > 0)
				{
					*buff++ = ':';
				}
				buff = Text::StrHexVal32V(buff, v);
			}
			i += 2;
		}
		if (lastIsSkip)
		{
			*buff++ = ':';
		}
		UInt32 zid = *(UInt32*)&addr->addr[16];
		if (zid != 0)
		{
			*buff++ = '%';
			buff = Text::StrUInt32(buff, zid);
		}
		*buff = 0;
		Text::StrToLower(oriBuff, oriBuff);
		return buff;
	}
	else
	{
		*buff = 0;
	}
	return 0;
}

WChar *Net::SocketUtil::GetAddrName(WChar *buff, const AddressInfo *addr)
{
	if (addr->addrType == AddrType::IPv4)
	{
		WChar *ptr;
		ptr = Text::StrInt32(buff, addr->addr[0]);
		*ptr++ = '.';
		ptr = Text::StrInt32(ptr, addr->addr[1]);
		*ptr++ = '.';
		ptr = Text::StrInt32(ptr, addr->addr[2]);
		*ptr++ = '.';
		ptr = Text::StrInt32(ptr, addr->addr[3]);
		return ptr;
	}
	else if (addr->addrType == AddrType::IPv6)
	{
		WChar *oriBuff = buff;
		Bool skipFound = false;
		Bool lastIsSkip = false;
		OSInt i = 0;
		UInt16 v;
		while (i < 16)
		{
			v = ReadMUInt16(&addr->addr[i]);
			if (v == 0)
			{
				if (!lastIsSkip)
				{
					if (skipFound)
					{
						*buff++ = ':';
						buff = Text::StrHexVal32V(buff, v);
					}
					else
					{
						lastIsSkip = true;
						skipFound = true;
						*buff++ = ':';
					}
				}
			}
			else
			{
				lastIsSkip = false;
				if (i > 0)
				{
					*buff++ = ':';
				}
				buff = Text::StrHexVal32V(buff, v);
			}
			i += 2;
		}
		if (lastIsSkip)
		{
			*buff++ = ':';
		}
		UInt32 zid = *(UInt32*)&addr->addr[16];
		if (zid != 0)
		{
			*buff++ = '%';
			buff = Text::StrUInt32(buff, zid);
		}
		*buff = 0;
		Text::StrToLower(oriBuff, oriBuff);
		return buff;
	}
	else
	{
		*buff = 0;
	}
	return 0;
}

UTF8Char *Net::SocketUtil::GetAddrName(UTF8Char *buff, const AddressInfo *addr, UInt16 port)
{
	if (addr->addrType == AddrType::IPv4)
	{
		return Text::StrUInt32(Text::StrConcat(GetAddrName(buff, addr), (const UTF8Char*)":"), port);
	}
	else if (addr->addrType == AddrType::IPv6)
	{
		*buff++ = '[';
		buff = GetAddrName(buff, addr);
		*buff++ = ']';
		*buff++ = ':';
		return Text::StrUInt32(buff, port);
	}
	else
	{
		return 0;
	}
}

WChar *Net::SocketUtil::GetAddrName(WChar *buff, const AddressInfo *addr, UInt16 port)
{
	if (addr->addrType == AddrType::IPv4)
	{
		return Text::StrUInt32(Text::StrConcat(GetAddrName(buff, addr), L":"), port);
	}
	else if (addr->addrType == AddrType::IPv6)
	{
		*buff++ = '[';
		buff = GetAddrName(buff, addr);
		*buff++ = ']';
		*buff++ = ':';
		return Text::StrUInt32(buff, port);
	}
	else
	{
		return 0;
	}
}

UTF8Char *Net::SocketUtil::GetIPv4Name(UTF8Char *buff, UInt32 ip)
{
	UTF8Char *ptr;
	ptr = Text::StrUInt16(buff, ((UInt8*)&ip)[0]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[1]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[2]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[3]);
	return ptr;
}

WChar *Net::SocketUtil::GetIPv4Name(WChar *buff, UInt32 ip)
{
	WChar *ptr;
	ptr = Text::StrUInt16(buff, ((UInt8*)&ip)[0]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[1]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[2]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[3]);
	return ptr;
}

UTF8Char *Net::SocketUtil::GetIPv4Name(UTF8Char *buff, UInt32 ip, UInt16 port)
{
	UTF8Char *ptr;
	ptr = Text::StrUInt16(buff, ((UInt8*)&ip)[0]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[1]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[2]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[3]);
	*ptr++ = ':';
	ptr = Text::StrUInt16(ptr, port);
	return ptr;
}

WChar *Net::SocketUtil::GetIPv4Name(WChar *buff, UInt32 ip, UInt16 port)
{
	WChar *ptr;
	ptr = Text::StrUInt16(buff, ((UInt8*)&ip)[0]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[1]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[2]);
	*ptr++ = '.';
	ptr = Text::StrUInt16(ptr, ((UInt8*)&ip)[3]);
	*ptr++ = ':';
	ptr = Text::StrUInt16(ptr, port);
	return ptr;
}

Bool Net::SocketUtil::GetIPAddr(const WChar *ipName, AddressInfo *addr)
{
	WChar sbuff[51];
	WChar *sarr[9];
	WChar *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 v;
	sptr = Text::StrConcatS(sbuff,ipName, 51);
	if ((sptr - sbuff) >= 50)
	{
		return false;
	}

	i = Text::StrSplit(sarr, 9, sbuff, ':');
	if (i >= 9)
	{
		return false;
	}
	else if (i == 1)
	{
		i = Text::StrSplit(sarr, 9, sbuff, '.');
		if (i != 4)
		{
			return false;
		}
		if (!Text::StrToUInt8(sarr[0], &addr->addr[0]) || !Text::StrToUInt8(sarr[1], &addr->addr[1]) || !Text::StrToUInt8(sarr[2], &addr->addr[2]) || !Text::StrToUInt8(sarr[3], &addr->addr[3]))
			return false;
		addr->addrType = AddrType::IPv4;
		return true;
	}
	j = Text::StrIndexOf(sarr[i - 1], '%');
	if (j != INVALID_INDEX)
	{
		if (Text::StrToInt32(&sarr[i - 1][j + 1], &v))
		{
			*(Int32*)&addr->addr[16] = v;
			sarr[i - 1][j] = 0;
		}
		else
		{
			return false;
		}
	}
	else
	{
		*(Int32*)&addr->addr[16] = 0;
	}
	if (sarr[0][0] == 0)
	{
		if (sarr[1][0] != 0)
			return false;
		if (i < 3)
			return false;
		if (i == 3 && sarr[2][0] == 0)
		{
			MemClear(addr->addr, 20);
			addr->addrType = AddrType::IPv6;
			return true;
		}
		j = 14;
		while (i-- > 2)
		{
			v = Text::StrHex2Int32C(sarr[i]);
			if (v > 65535)
			{
				return false;
			}
			if (v == 0)
			{
				if (sarr[i][0] != '0' || sarr[i][1] != 0)
				{
					return false;
				}
			}
			WriteMInt16(&addr->addr[j], v);
			j -= 2;
		}
		while (j >= 0)
		{
			WriteMInt16(&addr->addr[j], 0);
			j-= 2;
		}
		addr->addrType = AddrType::IPv6;
		return true;
	}
	j = 0;
	while (j < i)
	{
		if (sarr[j][0] == 0)
		{
			if (j + 1 >= i)
			{
				return false;
			}
			j++;
			k = 7;
			while (i-- > j)
			{
				v = Text::StrHex2Int32C(sarr[i]);
				if (v > 65535)
				{
					return false;
				}
				if (v == 0)
				{
					if (sarr[i][0] != '0' || sarr[i][1] != 0)
					{
						return false;
					}
				}
				WriteMInt16(&addr->addr[k << 1], v);
				k--;
			}
			while (k >= j - 1)
			{
				WriteMInt16(&addr->addr[k << 1], 0);
				k--;
			}
			addr->addrType = AddrType::IPv6;
			return true;
		}
		else
		{
			v = Text::StrHex2Int32C(sarr[j]);
			if (v > 65535)
			{
				return false;
			}
			if (v == 0)
			{
				if (sarr[j][0] != '0' || sarr[j][1] != 0)
				{
					return false;
				}
			}
			WriteMInt16(&addr->addr[j << 1], v);
		}
		j++;
	}
	if (i != 8)
	{
		return false;
	}
	addr->addrType = AddrType::IPv6;
	return true;
}

Bool Net::SocketUtil::GetIPAddr(const Char *ipName, AddressInfo *addr)
{
	return GetIPAddr((const UTF8Char*)ipName, addr);
}

Bool Net::SocketUtil::GetIPAddr(const UTF8Char *ipName, AddressInfo *addr)
{
	UTF8Char sbuff[51];
	UTF8Char *sarr[9];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 v;
	sptr = Text::StrConcatS(sbuff,ipName, 51);
	if ((sptr - sbuff) >= 50)
	{
		return false;
	}

	i = Text::StrSplit(sarr, 9, sbuff, ':');
	if (i >= 9)
	{
		return false;
	}
	else if (i == 1)
	{
		i = Text::StrSplit(sarr, 9, sbuff, '.');
		if (i != 4)
		{
			return false;
		}
		if (!Text::StrToUInt8(sarr[0], &addr->addr[0]) || !Text::StrToUInt8(sarr[1], &addr->addr[1]) || !Text::StrToUInt8(sarr[2], &addr->addr[2]) || !Text::StrToUInt8(sarr[3], &addr->addr[3]))
			return false;
		addr->addrType = AddrType::IPv4;
		return true;
	}
	j = Text::StrIndexOf(sarr[i - 1], '%');
	if (j != INVALID_INDEX)
	{
		if (Text::StrToInt32(&sarr[i - 1][j + 1], &v))
		{
			*(Int32*)&addr->addr[16] = v;
			sarr[i - 1][j] = 0;
		}
		else
		{
			return false;
		}
	}
	else
	{
		*(Int32*)&addr->addr[16] = 0;
	}
	if (sarr[0][0] == 0)
	{
		if (sarr[1][0] != 0)
			return false;
		if (i < 3)
			return false;
		if (i == 3 && sarr[2][0] == 0)
		{
			MemClear(addr->addr, 20);
			addr->addrType = AddrType::IPv6;
			return true;
		}
		j = 14;
		while (i-- > 2)
		{
			v = Text::StrHex2Int32C(sarr[i]);
			if (v > 65535)
			{
				return false;
			}
			if (v == 0)
			{
				if (sarr[i][0] != '0' || sarr[i][1] != 0)
				{
					return false;
				}
			}
			WriteMInt16(&addr->addr[j], v);
			j -= 2;
		}
		while ((OSInt)j >= 0)
		{
			WriteMInt16(&addr->addr[j], 0);
			j-= 2;
		}
		addr->addrType = AddrType::IPv6;
		return true;
	}
	j = 0;
	while (j < i)
	{
		if (sarr[j][0] == 0)
		{
			if (j + 1 >= i)
			{
				return false;
			}
			j++;
			k = 7;
			while (i-- > j)
			{
				v = Text::StrHex2Int32C(sarr[i]);
				if (v > 65535)
				{
					return false;
				}
				if (v == 0)
				{
					if (sarr[i][0] != '0' || sarr[i][1] != 0)
					{
						return false;
					}
				}
				WriteMInt16(&addr->addr[k << 1], v);
				k--;
			}
			while (k >= j - 1)
			{
				WriteMInt16(&addr->addr[k << 1], 0);
				k--;
			}
			addr->addrType = AddrType::IPv6;
			return true;
		}
		else
		{
			v = Text::StrHex2Int32C(sarr[j]);
			if (v > 65535)
			{
				return false;
			}
			if (v == 0)
			{
				if (sarr[j][0] != '0' || sarr[j][1] != 0)
				{
					return false;
				}
			}
			WriteMInt16(&addr->addr[j << 1], v);
		}
		j++;
	}
	if (i != 8)
	{
		return false;
	}
	addr->addrType = AddrType::IPv6;
	return true;
}

UInt32 Net::SocketUtil::GetIPAddr(const WChar *ipName)
{
	WChar sbuff[32];
	WChar *sarr[4];
	UInt8 ip[4];
	if ((Text::StrConcatS(sbuff, ipName, 30) - sbuff) > 15)
		return 0;
	if (Text::StrSplit(sarr, 4, sbuff, '.') != 4)
		return 0;
	if (!Text::StrToUInt8(sarr[0], &ip[0]) || !Text::StrToUInt8(sarr[1], &ip[1]) || !Text::StrToUInt8(sarr[2], &ip[2]) || !Text::StrToUInt8(sarr[3], &ip[3]))
		return 0;
	return *(UInt32*)ip;
}

UInt32 Net::SocketUtil::GetIPAddr(const Char *ipName)
{
	Char sbuff[32];
	Char *sarr[4];
	UInt8 ip[4];
	if (Text::StrCharCnt(ipName) > 31)
	{
		return 0;
	}
	Text::StrConcat(sbuff, ipName);
	if (Text::StrSplit(sarr, 4, sbuff, '.') != 4)
		return 0;
	ip[0] = (UInt8)Text::StrToInt32(sarr[0]);
	ip[1] = (UInt8)Text::StrToInt32(sarr[1]);
	ip[2] = (UInt8)Text::StrToInt32(sarr[2]);
	ip[3] = (UInt8)Text::StrToInt32(sarr[3]);
	return *(UInt32*)ip;
}

UInt32 Net::SocketUtil::GetIPAddr(const UTF8Char *ipName)
{
	return GetIPAddr((const Char*)ipName);
}

UInt32 Net::SocketUtil::GetDefNetMaskv4(UInt32 ip)
{
	UInt8 ipBytes[4];
	*(UInt32*)ipBytes = ip;
	if ((ipBytes[0] & 0x80) == 0)
	{
		ipBytes[0] = 0xff;
		ipBytes[1] = 0;
		ipBytes[2] = 0;
		ipBytes[3] = 0;
		return *(UInt32*)ipBytes;
	}
	else if ((ipBytes[0] & 0xc0) == 0x80)
	{
		ipBytes[0] = 0xff;
		ipBytes[1] = 0xff;
		ipBytes[2] = 0;
		ipBytes[3] = 0;
		return *(UInt32*)ipBytes;
	}
	else if ((ipBytes[0] & 0xe0) == 0xc0)
	{
		ipBytes[0] = 0xff;
		ipBytes[1] = 0xff;
		ipBytes[2] = 0xff;
		ipBytes[3] = 0;
		return *(UInt32*)ipBytes;
	}
	else
	{
		ipBytes[0] = 0xff;
		ipBytes[1] = 0xff;
		ipBytes[2] = 0xff;
		ipBytes[3] = 0xff;
		return *(UInt32*)ipBytes;
	}
}

void Net::SocketUtil::SetAddrInfoV4(AddressInfo *addr, UInt32 ipv4)
{
	addr->addrType = AddrType::IPv4;
	*(UInt32*)addr->addr = ipv4;
}

void Net::SocketUtil::SetAddrInfoV6(AddressInfo *addr, const UInt8 *ipv6, Int32 zid)
{
	addr->addrType = AddrType::IPv6;
	MemCopyNO(addr->addr, ipv6, 16);
	*(Int32*)&addr->addr[16] = zid;
}

void Net::SocketUtil::SetAddrAnyV6(AddressInfo *addr)
{
	addr->addrType = AddrType::IPv6;
	MemClear(addr->addr, 20);
}

UInt32 Net::SocketUtil::CalcCliId(const AddressInfo *addr)
{
	if (addr->addrType == AddrType::IPv4)
	{
		return *(UInt32*)addr->addr;
	}
	else if (addr->addrType == AddrType::IPv6)
	{
		Crypto::Hash::CRC32R crc;
		UInt8 ret[4];
		crc.Calc(addr->addr, 16);
		crc.GetValue(ret);
		return ReadMUInt32(ret);
	}
	return 0;
}

UInt32 Net::SocketUtil::IPv4ToSortable(UInt32 ipv4)
{
#if IS_BYTEORDER_LE == 1
	return BSWAPU32(ipv4);
#else
	return ipv4;
#endif
}

Bool Net::SocketUtil::IPv4SubnetValid(UInt32 subnet)
{
	if (subnet == 0)
		return false;
	UInt32 c;
	subnet = IPv4ToSortable(subnet);
	c = 1;
	while ((subnet & c) == 0)
	{
		c = c << 1;
	}
	while (c)
	{
		if ((subnet & c) == 0)
		{
			return false;
		}
		c = c << 1;
	}
	return true;
}

Net::IPType Net::SocketUtil::GetIPv4Type(UInt32 ipv4)
{
	UInt8 ipBuff[4];
	WriteNUInt32(ipBuff, ipv4);
	if (ipv4 == 0)
	{
		return Net::IPType::Network;
	}
	else if (ipv4 == 0xFFFFFFFF)
	{
		return Net::IPType::Broadcast;
	}
	else if (ipBuff[0] == 127)
	{
		return Net::IPType::Local;
	}
	else if (ipBuff[0] >= 224 && ipBuff[0] <= 239)
	{
		return Net::IPType::Multicast;
	}

	if (ipBuff[0] <= 127)
	{
		if (ipBuff[1] == 255 && ipBuff[2] == 255 && ipBuff[3] == 255)
		{
			return Net::IPType::Broadcast;
		}
		else if (ipBuff[1] == 0 && ipBuff[2] == 0 && ipBuff[3] == 0)
		{
			return Net::IPType::Network;
		}
	}
	else if (ipBuff[0] <= 191)
	{
		if (ipBuff[2] == 255 && ipBuff[3] == 255)
		{
			return Net::IPType::Broadcast;
		}
		else if (ipBuff[2] == 0 && ipBuff[3] == 0)
		{
			return Net::IPType::Network;
		}
	}
	else if (ipBuff[0] <= 223)
	{
		if (ipBuff[3] == 255)
		{
			return Net::IPType::Broadcast;
		}
		else if (ipBuff[3] == 0)
		{
			return Net::IPType::Network;
		}
	}
	
	if (ipBuff[0] == 10)
	{
		return Net::IPType::Private;
	}
	else if (ipBuff[0] == 172 && (ipBuff[1] & 0xf0) == 16)
	{
		return Net::IPType::Private;
	}
	else if (ipBuff[0] == 192 && ipBuff[1] == 168)
	{
		return Net::IPType::Private;
	}
	else
	{
		return Net::IPType::Public;
	}
}

UInt32 Net::SocketUtil::IPv4ToBroadcast(UInt32 ipv4)
{
	return ipv4 | ~GetDefNetMaskv4(ipv4);
}

Bool Net::SocketUtil::AddrEquals(const Net::SocketUtil::AddressInfo *addr1, const Net::SocketUtil::AddressInfo *addr2)
{
	if (addr1->addrType != addr2->addrType)
	{
		return false;
	}
	if (addr1->addrType == Net::AddrType::IPv4)
	{
		return ReadNInt32(addr1->addr) == ReadNInt32(addr2->addr);
	}
	else if (addr1->addrType == Net::AddrType::IPv6)
	{
		OSInt i = 20;
		while (i-- > 0)
		{
			if (addr1->addr[i] != addr2->addr[i])
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}
