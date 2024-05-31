#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/AddressRange.h"
#include "Text/MyString.h"

Net::AddressRange::AddressRange(UnsafeArray<const UTF8Char> addr, UOSInt addrLen, Bool scanBoardcast)
{
	this->skipFirst = false;
	this->skipLast = false;
	UTF8Char sbuff[64];
	UOSInt i = Text::StrIndexOfChar(addr, '/');
	if (i != INVALID_INDEX)
	{
		if (!Text::StrToUInt8(&addr[i + 1], this->param))
		{
			this->aType = AT_ERROR;
		}
		else if (i > 16)
		{
			this->aType = AT_ERROR;
		}
		else
		{
			Text::StrConcatC(sbuff, addr, i);
			if (!Net::SocketUtil::SetAddrInfo(this->addr1, Text::CStringNN(sbuff, i)))
			{
				this->aType = AT_ERROR;
			}
			else
			{
				this->aType = AT_MASK;
				if (this->addr1.addrType == Net::AddrType::IPv4)
				{
					UInt32 mask = ((UInt32)1 << (32 - this->param)) - 1;
					if (Net::SocketUtil::GetIPv4Type(ReadNUInt32(this->addr1.addr) | Net::SocketUtil::IPv4ToSortable(mask)) == Net::IPType::Broadcast)
					{
						this->skipLast = true;
					}
					if (Net::SocketUtil::GetIPv4Type(ReadNUInt32(this->addr1.addr) & Net::SocketUtil::IPv4ToSortable(~mask)) == Net::IPType::Network)
					{
						this->skipFirst = true;
					}
				}
			}
		}
		return;
	}

	i = Text::StrIndexOfChar(addr, '-');
	if (i != INVALID_INDEX)
	{
		if (!Text::StrToUInt8(&addr[i + 1], this->param))
		{
			this->aType = AT_ERROR;
		}
		else if (i > 16)
		{
			this->aType = AT_ERROR;
		}
		else
		{
			Text::StrConcatC(sbuff, addr, i);
			if (!Net::SocketUtil::SetAddrInfo(this->addr1, Text::CStringNN(sbuff, i)))
			{
				this->aType = AT_ERROR;
			}
			else if (this->addr1.addrType == Net::AddrType::IPv4)
			{
				UInt8 buff[4];
				buff[0] = this->addr1.addr[0];
				buff[1] = this->addr1.addr[1];
				buff[2] = this->addr1.addr[2];
				buff[3] = this->addr1.addr[3];
				if (Net::SocketUtil::GetIPv4Type(ReadNUInt32(buff)) == Net::IPType::Network)
				{
					this->skipFirst = true;
				}
				buff[3] = this->param;
				if (Net::SocketUtil::GetIPv4Type(ReadNUInt32(buff)) == Net::IPType::Broadcast)
				{
					this->skipLast = true;
				}
				this->aType = AT_RANGE;
			}
			else
			{
				this->aType = AT_ERROR;
			}
		}
		return;
	}
	if (Net::SocketUtil::SetAddrInfo(this->addr1, Text::CStringNN(addr, addrLen)))
	{
		if (this->addr1.addrType == Net::AddrType::IPv4)
		{
			Net::IPType ipType = Net::SocketUtil::GetIPv4Type(ReadNUInt32(this->addr1.addr));
			if (ipType == Net::IPType::Broadcast && scanBoardcast)
			{
				this->aType = AT_SCAN;
				this->skipFirst = true;
				this->skipLast = true;
			}
			else
			{
				this->aType = AT_SINGLE;
			}
		}
		else if (this->addr1.addrType == Net::AddrType::IPv6)
		{
			this->aType = AT_SINGLE;
		}
		else
		{
			this->aType = AT_ERROR;
		}
	}
	else
	{
		this->aType = AT_ERROR;
	}
}

Net::AddressRange::~AddressRange()
{

}

UOSInt Net::AddressRange::GetCount()
{
	UInt32 netMask;
	UOSInt cnt;
	switch (this->aType)
	{
	case AT_ERROR:
		return 0;
	case AT_SINGLE:
		return 1;
	case AT_RANGE:
		cnt = (UOSInt)this->param - this->addr1.addr[3] + 1;
		if (this->skipFirst)
		{
			cnt--;
		}
		if (this->skipLast)
		{
			cnt--;
		}
		return cnt;
	case AT_MASK:
		cnt = (UOSInt)1 << (32 - this->param);
		if (this->skipFirst)
		{
			cnt--;
		}
		if (this->skipLast)
		{
			cnt--;
		}
		return cnt;
	case AT_SCAN:
		netMask = Net::SocketUtil::IPv4ToSortable(Net::SocketUtil::GetDefNetMaskv4(ReadNUInt32(this->addr1.addr)));
		if (netMask == 0xff000000)
		{
			cnt = 0x1000000;
		}
		else if (netMask == 0xffff0000)
		{
			cnt = 0x10000;
		}
		else if (netMask == 0xffffff00)
		{
			cnt = 0x100;
		}
		else
		{
			return 1;
		}
		if (this->skipFirst)
		{
			cnt--;
		}
		if (this->skipLast)
		{
			cnt--;
		}
		return cnt;
	default:
		return 0;
	}
}

Bool Net::AddressRange::GetItem(UOSInt index, Net::SocketUtil::AddressInfo *addr)
{
	UInt32 netMask;
	UInt32 ip;
	UOSInt cnt;
	switch (this->aType)
	{
	case AT_ERROR:
		return false;
	case AT_SINGLE:
		if (index != 0)
		{
			return false;
		}
		*addr = this->addr1;
		return true;
	case AT_RANGE:
		cnt = (UOSInt)this->param - this->addr1.addr[3] + 1;
		if (this->skipFirst)
		{
			cnt--;
		}
		if (this->skipLast)
		{
			cnt--;
		}
		if (index >= cnt)
		{
			return false;
		}
		*addr = this->addr1;
		if (this->skipFirst)
		{
			addr->addr[3] = (UInt8)((UOSInt)addr->addr[3] + 1 + index);
		}
		else
		{
			addr->addr[3] = (UInt8)(addr->addr[3] + index);
		}
		return true;
	case AT_MASK:
		cnt = (UOSInt)1 << (32 - this->param);
		if (this->skipFirst)
		{
			cnt--;
		}
		if (this->skipLast)
		{
			cnt--;
		}
		if (index >= cnt)
		{
			return false;
		}
		ip = ReadMUInt32(this->addr1.addr);
		netMask = (UInt32)(1 << (32 - this->param)) - 1;
		ip = (ip & ~netMask) + (UInt32)index;
		if (this->skipFirst)
		{
			ip++;
		}
		addr->addrType = Net::AddrType::IPv4;
		WriteMUInt32(addr->addr, ip);
		return true;
	case AT_SCAN:
		ip = ReadMUInt32(this->addr1.addr);
		netMask = Net::SocketUtil::IPv4ToSortable(Net::SocketUtil::GetDefNetMaskv4(ReadNUInt32(this->addr1.addr)));
		if (netMask == 0xff000000)
		{
			cnt = 0x1000000;
		}
		else if (netMask == 0xffff0000)
		{
			cnt = 0x10000;
		}
		else if (netMask == 0xffffff00)
		{
			cnt = 0x100;
		}
		else
		{
			return 1;
		}
		if (this->skipFirst)
		{
			cnt--;
		}
		if (this->skipLast)
		{
			cnt--;
		}
		if (index >= cnt)
		{
			return false;
		}
		ip = (ip & netMask) + (UInt32)index;
		if (this->skipFirst)
		{
			ip++;
		}
		addr->addrType = Net::AddrType::IPv4;
		WriteMUInt32(addr->addr, ip);
		return true;
	default:
		return false;
	}
}
