#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ARPInfo.h"
#include "Net/ConnectionInfo.h"
#include "Net/ICMPScanner.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

#define THREADLEV 5

typedef struct
{
	Net::ICMPScanner *me;
	UInt32 ipStart;
	UInt32 ipEnd;
	Bool ended;
	Sync::Event *evt;
} PingStatus;

void Net::ICMPScanner::ICMPChecksum(UInt8 *buff, OSInt buffSize)
{
	UInt8 *oriBuff = buff;
    UInt32 sum = 0xffff;
    while (buffSize > 1)
	{
        sum += ReadUInt16(buff);
        buff += 2;
        buffSize -= 2;
    }

    if(buffSize == 1)
        sum += buff[0];

    sum = (sum & 0xffff) + (sum >> 16);
    sum = (sum & 0xffff) + (sum >> 16);

	WriteInt16(&oriBuff[2], ~sum);
}

UInt32 __stdcall Net::ICMPScanner::Ping1Thread(AnyType userObj)
{
	NN<PingStatus> status = userObj.GetNN<PingStatus>();
	UInt8 buff1[4];
	UInt8 buff2[4];
	NN<ScanResult> result;
	Net::SocketUtil::AddressInfo addr;
	UInt32 respTime;
	UInt32 ttl;
	WriteNUInt32(buff1, status->ipStart);
	WriteNUInt32(buff2, status->ipEnd);

	while (buff1[3] <= buff2[3])
	{
		if (buff1[3] != 0 && buff1[3] != 255)
		{
			Net::SocketUtil::SetAddrInfoV4(addr, ReadNUInt32(buff1));
			if (status->me->sockf->IcmpSendEcho2(addr, &respTime, &ttl))
			{
				result = MemAllocNN(ScanResult);
				result->ip = ReadNUInt32(buff1);
				result->respTime = respTime / 1000000.0;
				result->mac[0] = 0;
				result->mac[1] = 0;
				result->mac[2] = 0;
				result->mac[3] = 0;
				result->mac[4] = 0;
				result->mac[5] = 0;
				Sync::MutexUsage mutUsage(status->me->resultMut);
				status->me->results.Put(Net::SocketUtil::IPv4ToSortable(result->ip), result);
				mutUsage.EndUse();
			}
		}
		if (buff1[3] == buff2[3])
		{
			break;
		}
		buff1[3]++;
	}
	status->ended = true;
	status->evt->Set();
	return 0;
}

UInt32 __stdcall Net::ICMPScanner::Ping2Thread(AnyType userObj)
{
	NN<Net::ICMPScanner> me = userObj.GetNN<Net::ICMPScanner>();
	UInt8 *readBuff;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Net::SocketFactory::ErrorType et;
	UOSInt readSize;
	NN<ScanResult> result;
	readBuff = MemAlloc(UInt8, 4096);
	UInt8 *ipData;
	UOSInt ipDataSize;
	while (!me->threadToStop)
	{
		readSize = me->sockf->UDPReceive(me->soc, readBuff, 4096, addr, port, et);
		if (readSize >= 36)
		{
			if ((readBuff[0] & 0xf0) == 0x40 && readBuff[9] == 1)
			{
				if ((readBuff[0] & 0xf) <= 5)
				{
					ipData = &readBuff[20];
					ipDataSize = readSize - 20;
				}
				else
				{
					ipData = &readBuff[(readBuff[0] & 0xf) << 2];
					ipDataSize = readSize - ((readBuff[0] & 0xf) << 2);
				}

				if (ipData[0] == 0 && ipDataSize >= 8)
				{
					Sync::MutexUsage mutUsage(me->resultMut);
					if (!me->results.Get(ReadMUInt32(&readBuff[12])).SetTo(result))
					{
						result = MemAllocNN(ScanResult);
						result->ip = ReadNUInt32(&readBuff[12]);
						result->respTime = me->clk->GetTimeDiff();;
						result->mac[0] = 0;
						result->mac[1] = 0;
						result->mac[2] = 0;
						result->mac[3] = 0;
						result->mac[4] = 0;
						result->mac[5] = 0;
						me->results.Put(ReadMUInt32(&readBuff[12]), result);
					}
					mutUsage.EndUse();
				}
			}
		}
	}
	MemFree(readBuff);
	me->threadRunning = false;
	return 0;
}

void Net::ICMPScanner::AppendMACs(UInt32 ip)
{
	Data::ArrayListNN<Net::ARPInfo> arpList;
	NN<Net::ARPInfo> arp;
	UInt8 mac[6];
	UInt32 ipAddr;
	UOSInt i;
	UOSInt j;
	NN<ScanResult> result;
	Net::ARPInfo::GetARPInfoList(arpList);
	i = 0;
	j = arpList.GetCount();
	while (i < j)
	{
		arp = arpList.GetItemNoCheck(i);
		ipAddr = arp->GetIPAddress();
		if (arp->GetARPType() != Net::ARPInfo::ARPT_INVALID && arp->GetPhysicalAddr(mac) == 6)
		{
			if (this->results.Get(Net::SocketUtil::IPv4ToSortable(ipAddr)).SetTo(result))
			{
				MemCopyNO(result->mac, mac, 6);
			}
		}
		arp.Delete();
		i++;
	}

	Data::ArrayListNN<Net::ConnectionInfo> connList;
	NN<Net::ConnectionInfo> conn;
	this->sockf->GetConnInfoList(connList);
	i = connList.GetCount();
	while (i-- > 0)
	{
		conn = connList.GetItemNoCheck(i);
		j = 0;
		while (true)
		{
			ipAddr = conn->GetIPAddress(j);
			if (ipAddr == 0)
			{
				break;
			}
			if (ipAddr == ip)
			{
				if (conn->GetPhysicalAddress(mac, 6) == 6)
				{
					if (this->results.Get(Net::SocketUtil::IPv4ToSortable(ipAddr)).SetTo(result))
					{
						MemCopyNO(result->mac, mac, 6);
					}
				}
				break;
			}
			j++;
		}
		conn.Delete();
	}
}

Net::ICMPScanner::ICMPScanner(NN<Net::SocketFactory> sockf)
{
	this->sockf = sockf;
}

Net::ICMPScanner::~ICMPScanner()
{
	this->ClearResults();
}

Bool Net::ICMPScanner::Scan(UInt32 ip)
{
	UInt8 buff[8];
	UInt8 packetBuff[64];
	Socket *s;
	WriteNUInt32(buff, ip);
	if (buff[0] == 192 && buff[1] == 168)
	{

	}
	else
	{
		return false;
	}

	this->ClearResults();

	if (0)
	{
	}
	/*		if ((s = me->sockf->CreateRAWSocket()) != 0)
	{
		me->sockf->DestroySocket(s);
	}*/
	else if ((s = this->sockf->CreateICMPIPv4Socket(ip)) != 0)
	{
		this->threadRunning = true;
		this->threadToStop = false;
		this->soc = s;
		NEW_CLASS(this->clk, Manage::HiResClock());
		Sync::ThreadUtil::Create(Ping2Thread, this);

		Net::SocketUtil::AddressInfo addr;
		packetBuff[0] = 8; //type = Echo request
		packetBuff[1] = 0; //code = 0
		WriteNInt16(&packetBuff[2], 0); //checksum = 0;
		WriteNInt16(&packetBuff[4], 0); //id = 0;
		WriteNInt16(&packetBuff[6], 0); //seq = 0;
		MemClear(&packetBuff[8], 56);
		ICMPChecksum(packetBuff, 64);

		this->clk->Start();
		buff[3] = 1;
		while (buff[3] < 255)
		{
			Net::SocketUtil::SetAddrInfoV4(addr, ReadNUInt32(buff));
			this->sockf->SendTo(s, packetBuff, 64, addr, 0);
			buff[3]++;
		}

		Sync::SimpleThread::Sleep(3000);
		this->threadToStop = true;
		this->sockf->DestroySocket(s);
		while (this->threadRunning)
		{
			Sync::SimpleThread::Sleep(1);
		}
		this->soc = 0;
		DEL_CLASS(this->clk);
		this->clk = 0;

		this->AppendMACs(ip);
	}
	else
	{
		PingStatus *status = MemAlloc(PingStatus, (1 << THREADLEV));
		NEW_CLASS(status[0].evt, Sync::Event(true));
		UOSInt i = 0;
		while (i < (UOSInt)(1 << THREADLEV))
		{
			status[i].me = this;
			status[i].evt = status[0].evt;
			status[i].ended = false;
			buff[3] = (UInt8)(i << (8 - THREADLEV));
			status[i].ipStart = ReadNUInt32(buff);
			buff[3] = (UInt8)(buff[3] + (1 << (8 - THREADLEV)) - 1);
			status[i].ipEnd = ReadNUInt32(buff);
			Sync::ThreadUtil::Create(Ping1Thread, &status[i]);
			i++;
		}

		Bool running;
		while (true)
		{
			running = false;
			i = 1 << THREADLEV;
			while (i-- > 0)
			{
				if (!status[i].ended)
				{
					running = true;
					break;
				}
			}
			if (running)
			{
				status[0].evt->Wait(1000);
			}
			else
			{
				break;
			}
		}
		DEL_CLASS(status[0].evt);
		MemFree(status);

		this->AppendMACs(ip);
	}
	return true;
}

NN<const Data::ReadingListNN<Net::ICMPScanner::ScanResult>> Net::ICMPScanner::GetResults() const
{
	return this->results;
}

void Net::ICMPScanner::ClearResults()
{
	NN<ScanResult> result;
	UOSInt i = this->results.GetCount();
	while (i-- > 0)
	{
		result = this->results.GetItemNoCheck(i);
		MemFreeNN(result);
	}
	this->results.Clear();
}
