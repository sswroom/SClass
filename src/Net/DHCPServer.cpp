#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ConnectionInfo.h"
#include "Net/DHCPServer.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

void __stdcall Net::DHCPServer::PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::DHCPServer *me = (Net::DHCPServer*)userData;
	UInt8 repBuff[512];
	Net::SocketUtil::AddressInfo destAddr;
	if (dataSize >= 240 && buff[0] == 1 && buff[1] == 1 && buff[2] == 6 && ReadMUInt32(&buff[236]) == 0x63825363)
	{
		UInt8 dhcpType = 0;
		UInt64 hwAddr = ReadMUInt64(&buff[26]) & 0xffffffffffffLL;
		UInt32 reqIP = 0;
		UInt32 transactionId = ReadMUInt32(&buff[4]);
		UOSInt i;
		UInt32 j;
		UInt8 t;
		UInt8 len;
		DeviceStatus *dev;
		Text::StringBuilderUTF8 sbHostName;
		Text::StringBuilderUTF8 sbVendorClass;
		i = 240;
		while (i < dataSize)
		{
			t = buff[i];
			if (t == 255)
				break;
			i++;
			if (i >= dataSize)
			{
				break;
			}
			len = buff[i];
			i++;
			if (i + len > dataSize)
			{
				break;
			}
			if (t == 12 && len >= 1)
			{
				sbHostName.ClearStr();
				sbHostName.AppendC(&buff[i], len);
			}
			else if (t == 53 && len == 1)
			{
				dhcpType = buff[i];
			}
			else if (t == 50 && len == 4)
			{
				reqIP = ReadNUInt32(&buff[i]);
			}
			else if (t == 60 && len >= 1)
			{
				sbVendorClass.ClearStr();
				sbVendorClass.AppendC(&buff[i], len);
			}
			
			i += len;
		}
		
		if (dhcpType == 1)
		{
			MemClear(repBuff, 240);
			repBuff[0] = 2;
			repBuff[1] = 1;
			repBuff[2] = 6;
			WriteMUInt32(&repBuff[4], transactionId);
			WriteMUInt64(&repBuff[26], hwAddr);
			WriteNUInt32(&repBuff[20], me->infIP);
			Sync::MutexUsage mutUsage(&me->devMut);
			dev = me->devMap.Get(hwAddr);
			if (dev)
			{
				reqIP = dev->assignedIP;
			}
			else
			{
				dev = MemAlloc(DeviceStatus, 1);
				dev->hwAddr = hwAddr;
				dev->hostName = 0;
				dev->vendorClass = 0;
				dev->updated = true;
				if (sbHostName.GetLength() > 0)
				{
					dev->hostName = Text::String::New(sbHostName.ToCString()).Ptr();
				}
				if (sbVendorClass.GetLength() > 0)
				{
					dev->vendorClass = Text::String::New(sbVendorClass.ToCString()).Ptr();
				}
				if (reqIP == 0 || (reqIP & me->subnet) != (me->infIP & me->subnet))
				{
					reqIP = 0;
					j = 0;
					while (j < me->devCount)
					{
						if (me->devUsed[j] == 0)
						{
							me->devUsed[j] = 1;
							reqIP = (me->infIP & me->subnet) | Net::SocketUtil::IPv4ToSortable(me->firstIP + j);
							break;
						}
						j++;
					}
					dev->assignedIP = reqIP;
				}
				else
				{
					j = Net::SocketUtil::IPv4ToSortable(reqIP & (~me->subnet)) - me->firstIP;
					if (me->devUsed[j] == 0)
					{
						me->devUsed[j] = 1;
						dev->assignedIP = reqIP;
					}
					else
					{
						reqIP = 0;
						j = 0;
						while (j < me->devCount)
						{
							if (me->devUsed[j] == 0)
							{
								me->devUsed[j] = 1;
								reqIP = (me->infIP & me->subnet) | Net::SocketUtil::IPv4ToSortable(me->firstIP + j);
								break;
							}
							j++;
						}
						dev->assignedIP = reqIP;
					}
				}
				Data::DateTime dt;
				dt.SetCurrTimeUTC();
				dev->assignTime = dt.ToTicks();
				me->devMap.Put(hwAddr, dev);
			}
			mutUsage.EndUse();
			WriteNUInt32(&repBuff[16], reqIP);
			WriteMInt32(&repBuff[236], 0x63825363);
			i = 240;
			repBuff[i] = 53;
			repBuff[i + 1] = 1;
			repBuff[i + 2] = 2; //Offer
			i += 3;
			repBuff[i] = 1;
			repBuff[i + 1] = 4;
			WriteNUInt32(&repBuff[i + 2], me->subnet);
			i += 6;
			repBuff[i] = 51;
			repBuff[i + 1] = 4;
			WriteMUInt32(&repBuff[i + 2], me->ipLeaseTime);
			i += 6;
			repBuff[i] = 54;
			repBuff[i + 1] = 4;
			WriteNUInt32(&repBuff[i + 2], me->infIP);
			i += 6;
			if (me->gateway)
			{
				repBuff[i] = 3;
				repBuff[i + 1] = 4;
				WriteNUInt32(&repBuff[i + 2], me->gateway);
				i += 6;
			}
			if (me->dnsList.GetCount() > 0)
			{
				repBuff[i] = 6;
				repBuff[i + 1] = (UInt8)(4 * me->dnsList.GetCount());
				i += 2;
				j = 0;
				while (j < me->dnsList.GetCount())
				{
					WriteNUInt32(&repBuff[i], me->dnsList.GetItem(j));
					j++;
					i += 4;
				}
			}
			repBuff[i] = 255;
			i++;

//			me->sockf->ARPAddRecord(me->infIndex, &buff[28], reqIP);
			Net::SocketUtil::SetAddrInfoV4(&destAddr, reqIP | (~me->subnet));
			me->svr->SendTo(&destAddr, 68, repBuff, i);
		}
		else if (dhcpType == 3)
		{
			MemClear(repBuff, 240);
			repBuff[0] = 2;
			repBuff[1] = 1;
			repBuff[2] = 6;
			WriteMUInt32(&repBuff[4], transactionId);
			WriteMUInt64(&repBuff[26], hwAddr);
			Sync::MutexUsage mutUsage(&me->devMut);
			dev = me->devMap.Get(hwAddr);
			if (dev == 0)
			{
				mutUsage.EndUse();
				return;
			}
			if (reqIP != dev->assignedIP)
			{
				mutUsage.EndUse();
				return;
			}
			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			dev->assignTime = dt.ToTicks();
			dev->updated = true;
			mutUsage.EndUse();
			WriteNUInt32(&repBuff[16], reqIP);
			WriteMInt32(&repBuff[236], 0x63825363);
			i = 240;
			repBuff[i] = 53;
			repBuff[i + 1] = 1;
			repBuff[i + 2] = 5; //ACK
			i += 3;
			repBuff[i] = 1;
			repBuff[i + 1] = 4;
			WriteNUInt32(&repBuff[i + 2], me->subnet);
			i += 6;
			repBuff[i] = 51;
			repBuff[i + 1] = 4;
			WriteMUInt32(&repBuff[i + 2], me->ipLeaseTime);
			i += 6;
			repBuff[i] = 54;
			repBuff[i + 1] = 4;
			WriteNUInt32(&repBuff[i + 2], me->infIP);
			i += 6;
			if (me->gateway)
			{
				repBuff[i] = 3;
				repBuff[i + 1] = 4;
				WriteNUInt32(&repBuff[i + 2], me->gateway);
				i += 6;
			}
			if (me->dnsList.GetCount() > 0)
			{
				repBuff[i] = 6;
				repBuff[i + 1] = (UInt8)(4 * me->dnsList.GetCount());
				i += 2;
				j = 0;
				while (j < me->dnsList.GetCount())
				{
					WriteNUInt32(&repBuff[i], me->dnsList.GetItem(j));
					j++;
					i += 4;
				}
			}
			repBuff[i] = 255;
			i++;

//			me->sockf->ARPAddRecord(me->infIndex, &buff[28], reqIP);
			Net::SocketUtil::SetAddrInfoV4(&destAddr, reqIP | (~me->subnet));
			me->svr->SendTo(&destAddr, 68, repBuff, i);
		}
	}
}

Net::DHCPServer::DHCPServer(NotNullPtr<Net::SocketFactory> sockf, UInt32 infIP, UInt32 subnet, UInt32 firstIP, UInt32 devCount, UInt32 gateway, Data::ArrayList<UInt32> *dnsList)
{
	this->sockf = sockf;
	this->svr = 0;
	this->infIndex = 0;
	this->infIP = infIP;
	this->subnet = subnet;
	this->firstIP = firstIP;
	this->devCount = devCount;
	this->gateway = gateway;
	this->dnsList.AddAll(dnsList);
	this->ipLeaseTime = 86400;

	this->devUsed = 0;

	UOSInt i;
	UOSInt j;
	UInt32 ip;
	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	this->sockf->GetConnInfoList(&connInfoList);
	i = connInfoList.GetCount();
	while (i-- > 0)
	{
		connInfo = connInfoList.GetItem(i);
		j = 0;
		while ((ip = connInfo->GetIPAddress(j)) != 0)
		{
			if (ip == infIP)
			{
				this->infIndex = (UInt32)connInfo->GetIndex();
				break;
			}
			j++;
		}
		DEL_CLASS(connInfo);
	}

	UInt32 lastIP = firstIP + devCount;
	if (!Net::SocketUtil::IPv4SubnetValid(subnet))
	{
		printf("Subnet not valid\r\n");
		return;
	}
	if (firstIP == 0 || devCount == 0 || Net::SocketUtil::IPv4ToSortable(subnet) < lastIP)
	{
		printf("Parameter validation failed\r\n");
		return;
	}
	if (gateway != 0)
	{
		if ((this->infIP & subnet) != (this->gateway & subnet))
		{
			printf("Gateway not valid\r\n");
			return;
		}
	}

	this->devUsed = MemAlloc(UInt8, this->devCount);
	MemClear(this->devUsed, this->devCount);
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfoV4(&addr, infIP);
	NEW_CLASS(this->svr, Net::UDPServer(this->sockf, &addr, 67, CSTR_NULL, PacketHdlr, this, 0, CSTR_NULL, 2, false));
}

Net::DHCPServer::~DHCPServer()
{
	if (this->svr)
	{
		DEL_CLASS(this->svr);
		this->svr = 0;

		DeviceStatus *dev;
		UOSInt i = this->devMap.GetCount();
		while (i-- > 0)
		{
			dev = this->devMap.GetItem(i);
			SDEL_STRING(dev->hostName);
			SDEL_STRING(dev->vendorClass);
			MemFree(dev);
		}
		MemFree(this->devUsed);
	}
}

Bool Net::DHCPServer::IsError() const
{
	if (this->svr == 0)
	{
		return true;
	}
	return this->svr->IsError();
}

void Net::DHCPServer::UseStatus(Sync::MutexUsage *mutUsage) const
{
	mutUsage->ReplaceMutex(&this->devMut);
}

const Data::ReadingList<Net::DHCPServer::DeviceStatus*> *Net::DHCPServer::StatusGetList() const
{
	return &this->devMap;
}

UInt32 Net::DHCPServer::GetIPLeaseTime() const
{
	return this->ipLeaseTime;
}
