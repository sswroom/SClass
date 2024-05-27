#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/TraceRoute.h"

void __stdcall Net::TraceRoute::RecvThread(NN<Sync::Thread> thread)
{
	NN<Net::TraceRoute> me = thread->GetUserObj().GetNN<Net::TraceRoute>();
	UInt8 *readBuff;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Net::SocketFactory::ErrorType et;
	NN<Socket> soc;
	UOSInt readSize;
	readBuff = MemAlloc(UInt8, 4096);
	UInt8 *ipData;
	UOSInt ipDataSize;
	me->resEvt->Set();
	if (me->socV4.SetTo(soc))
	{
		while (!thread->IsStopping())
		{
			readSize = me->sockf->UDPReceive(soc, readBuff, 4096, addr, port, et);
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
						UInt16 id = ReadMUInt16(&ipData[4]);
						UInt16 seq = ReadMUInt16(&ipData[6]);
						if (me->resId == id && me->resSeq == seq)
						{
							me->resIP = ReadNUInt32(&readBuff[12]);
							me->resFound = true;
							me->resEvt->Set();
						}
					}
					else if (ipData[0] == 8)
					{
					}
					else if (ipData[0] == 11)
					{
						me->resIP = ReadNUInt32(&readBuff[12]);
						me->resFound = true;
						me->resEvt->Set();
					}
				}
			}
		}
	}
	MemFree(readBuff);
}

void Net::TraceRoute::ICMPChecksum(UInt8 *buff, UOSInt buffSize)
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


Net::TraceRoute::TraceRoute(NN<Net::SocketFactory> sockf, UInt32 ip) : thread(RecvThread, this, CSTR("TraceRoute"))
{
	this->sockf = sockf;
	this->socV4 = this->sockf->CreateICMPIPv4Socket(ip);
	NEW_CLASSNN(this->resEvt, Sync::Event(true));
	if (this->socV4.NotNull())
	{
		this->thread.Start();
	}
}

Net::TraceRoute::~TraceRoute()
{
	this->thread.BeginStop();
	NN<Socket> soc;
	if (this->socV4.SetTo(soc))
	{
		this->sockf->DestroySocket(soc);
	}
	this->thread.WaitForEnd();
	this->resEvt.Delete();
	this->socV4 = 0;
}

Bool Net::TraceRoute::IsError()
{
	return this->socV4.IsNull();
}

Bool Net::TraceRoute::Tracev4(UInt32 ip, Data::ArrayList<UInt32> *ipList)
{
	NN<Socket> soc;
	if (!this->socV4.SetTo(soc))
		return false;
	UInt8 packetBuff[72];
	Net::SocketUtil::AddressInfo addr;
	packetBuff[0] = 8; //type = Echo request
	packetBuff[1] = 0; //code = 0
	WriteNInt16(&packetBuff[2], 0); //checksum = 0;
	WriteMInt16(&packetBuff[4], 1); //id = 0;
	WriteMInt16(&packetBuff[6], 0xd7); //seq = 0;
	MemClear(&packetBuff[8], 64);
	ICMPChecksum(packetBuff, 72);

	this->resId = 1;
	this->resSeq = 0xd7;
	this->resFound = false;

	Net::SocketUtil::SetAddrInfoV4(addr, ip);
	this->sockf->SetIPv4TTL(soc, 64);
	this->sockf->SendTo(soc, packetBuff, 72, addr, 0);

	this->resEvt->Wait(2000);
	if (!this->resFound)
	{
		return false;
	}
	UInt16 reqId = 1;
	UInt16 seq = 0xd8;
	Int32 ttl = 1;
	OSInt retry = 0;
	while (ttl < 32)
	{
		packetBuff[0] = 8; //type = Echo request
		packetBuff[1] = 0; //code = 0
		WriteNInt16(&packetBuff[2], 0); //checksum = 0;
		WriteMInt16(&packetBuff[4], reqId);
		WriteMInt16(&packetBuff[6], seq);
		MemClear(&packetBuff[8], 64);
		ICMPChecksum(packetBuff, 72);

		this->sockf->SetIPv4TTL(soc, ttl);
		this->resId = reqId;
		this->resSeq = seq;
		this->resFound = false;

		this->resEvt->Clear();
		this->sockf->SendTo(soc, packetBuff, 72, addr, 0);
		this->resEvt->Wait(2000);
		if (!this->resFound)
		{
			retry++;
			if (retry >= 1)
			{
				ipList->Add(0);
				ttl++;
				retry = 0;
			}
		}
		else
		{
			ipList->Add(this->resIP);
			if (this->resIP == ip)
				break;
			ttl++;
			retry = 0;
		}
		seq++;
	}
	if (ttl == 32)
	{
		return false;
	}
	return true;
}
