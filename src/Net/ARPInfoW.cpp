#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ARPInfo.h"
#include <winsock2.h>
#include <iphlpapi.h>

Net::ARPInfo::ARPInfo(void *ipNetRow)
{
	MIB_IPNETROW *row = (MIB_IPNETROW *)ipNetRow;
	this->ifIndex = row->dwIndex;
	this->phyAddr = MemAlloc(UInt8, row->dwPhysAddrLen);
	this->phyAddrLen = row->dwPhysAddrLen;
	MemCopyNO(this->phyAddr, row->bPhysAddr, this->phyAddrLen);
	this->ipAddr = row->dwAddr;
	this->arpType = (ARPType)row->dwType;
}

Net::ARPInfo::~ARPInfo()
{
	MemFree(this->phyAddr);
}

UInt32 Net::ARPInfo::GetAdaptorIndex()
{
	return this->ifIndex;
}

OSInt Net::ARPInfo::GetPhysicalAddr(UInt8 *buff)
{
	MemCopyNO(buff, this->phyAddr, this->phyAddrLen);
	return this->phyAddrLen;
}

UInt32 Net::ARPInfo::GetIPAddress()
{
	return this->ipAddr;
}

Net::ARPInfo::ARPType Net::ARPInfo::GetARPType()
{
	return this->arpType;
}

OSInt Net::ARPInfo::GetARPInfoList(Data::ArrayList<Net::ARPInfo*> *arpInfoList)
{
	UInt32 size;
	OSInt cnt;
	OSInt i;
	size = 0;
	if (GetIpNetTable(0, (ULONG*)&size, TRUE) == ERROR_INSUFFICIENT_BUFFER)
	{
		Net::ARPInfo *arp;
		MIB_IPNETTABLE *addrTable = (MIB_IPNETTABLE*)MAlloc(size);
		cnt = 0;
		if (GetIpNetTable(addrTable, (ULONG*)&size, TRUE) == NO_ERROR)
		{
			i = 0;
			cnt = addrTable->dwNumEntries;
			while (i < cnt)
			{
				NEW_CLASS(arp, Net::ARPInfo(&addrTable->table[i]));
				arpInfoList->Add(arp);
				i++;
			}
		}
		MemFree(addrTable);
		return cnt;
	}
	return 0;
}
