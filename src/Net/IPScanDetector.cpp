#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Net/IPScanDetector.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

#define TIMEDETECTRANGE 3000
#define DETECTCOUNT 5

UInt32 __stdcall Net::IPScanDetector::DataThread(AnyType obj)
{
	NN<Net::IPScanDetector::ThreadStat> stat = obj.GetNN<Net::IPScanDetector::ThreadStat>();
	UInt8 macBuff[8];
	stat->threadRunning = true;
	stat->me->ctrlEvt->Set();
	NN<Socket> soc;
	if (stat->me->soc.SetTo(soc))
	{
		UInt8 *buff = MemAlloc(UInt8, 2048);
		while (!stat->toStop)
		{
			UOSInt recvSize;
			Net::SocketUtil::AddressInfo recvAddr;
			UInt16 recvPort;

			recvSize = stat->me->sockf->UDPReceive(soc, buff, 2048, recvAddr, recvPort, 0);
			if (recvSize >= 42)
			{
				UInt16 opcode = ReadMUInt16(&buff[20]);
				if (opcode == 1) //Request
				{
					UInt32 ip;
					Int64 reqTime;
					Int64 lastTime;
					UInt64 iMAC;
					NN<Net::IPScanDetector::AdapterStatus> adapter;
					Data::DateTime dt;
					dt.SetCurrTimeUTC();
					reqTime = dt.ToTicks();
					macBuff[0] = 0;
					macBuff[1] = 0;
					MemCopyNO(&macBuff[2], &buff[6], 6);
					iMAC = ReadMUInt64(macBuff);
					ip = ReadMUInt32(&buff[38]);
					Sync::MutexUsage mutUsage(stat->me->adapterMut);
					if (stat->me->adapterMap.Get(iMAC).SetTo(adapter))
					{
						if (adapter->lastDetectTime + TIMEDETECTRANGE < reqTime)
						{
							adapter->detectCnt = 1;
						}
						else
						{
							lastTime = adapter->targetIPMap.Get(ip);
							if (lastTime == 0 || lastTime + TIMEDETECTRANGE < reqTime)
							{
								adapter->detectCnt++;
								if (adapter->detectCnt == DETECTCOUNT)
								{
									stat->me->hdlr(&macBuff[2], stat->me->userData);
								}
							}
						}
						adapter->targetIPMap.Put(ip, reqTime);
						adapter->lastDetectTime = reqTime;
					}
					else
					{
						NEW_CLASSNN(adapter, Net::IPScanDetector::AdapterStatus());
						adapter->iMAC = iMAC;
						adapter->lastDetectTime = reqTime;
						adapter->detectCnt = 1;
						adapter->targetIPMap.Put(ip, reqTime);
						stat->me->adapterMap.Put(iMAC, adapter);
					}
					mutUsage.EndUse();
				}
				else if (opcode == 2) //Reply
				{
					UInt32 ip;
					Int64 reqTime;
					Int64 lastTime;
					UInt64 iMAC;
					NN<Net::IPScanDetector::AdapterStatus> adapter;
					Data::DateTime dt;
					dt.SetCurrTimeUTC();
					reqTime = dt.ToTicks();
					macBuff[0] = 0;
					macBuff[1] = 0;
					MemCopyNO(&macBuff[2], &buff[0], 6);
					iMAC = ReadMUInt64(macBuff);
					ip = ReadMUInt32(&buff[28]);
					Sync::MutexUsage mutUsage(stat->me->adapterMut);
					if (stat->me->adapterMap.Get(iMAC).SetTo(adapter))
					{
						if (adapter->lastDetectTime + TIMEDETECTRANGE < reqTime)
						{
							adapter->detectCnt = 1;
						}
						else
						{
							lastTime = adapter->targetIPMap.Get(ip);
							if (lastTime == 0 || lastTime + TIMEDETECTRANGE < reqTime)
							{
								adapter->detectCnt++;
								if (adapter->detectCnt == DETECTCOUNT)
								{
									stat->me->hdlr(&macBuff[2], stat->me->userData);
								}
							}
						}
						adapter->targetIPMap.Put(ip, reqTime);
						adapter->lastDetectTime = reqTime;
					}
					else
					{
						NEW_CLASSNN(adapter, Net::IPScanDetector::AdapterStatus());
						adapter->iMAC = iMAC;
						adapter->lastDetectTime = reqTime;
						adapter->detectCnt = 1;
						adapter->targetIPMap.Put(ip, reqTime);
						stat->me->adapterMap.Put(iMAC, adapter);
					}
					mutUsage.EndUse();
				}
			}
		}
		MemFree(buff);
	}
	stat->threadRunning = false;
	stat->me->ctrlEvt->Set();
	return 0;
}

Net::IPScanDetector::IPScanDetector(NN<Net::SocketFactory> sockf, IPScanHandler hdlr, AnyType userData, UOSInt threadCnt)
{
	UOSInt i;
	this->threadCnt = threadCnt;
	this->sockf = sockf;
	this->hdlr = hdlr;
	this->userData = userData;
	this->ctrlEvt = 0;
	this->soc = this->sockf->CreateARPSocket();

	NN<Socket> soc;
	if (this->soc.SetTo(soc))
	{
		NEW_CLASS(this->ctrlEvt, Sync::Event(true));

		this->threadStats = MemAlloc(Net::IPScanDetector::ThreadStat, this->threadCnt);

		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStats[i].toStop = false;
			this->threadStats[i].threadRunning = false;
			NEW_CLASS(this->threadStats[i].evt, Sync::Event(true));
			this->threadStats[i].me = this;
			Sync::ThreadUtil::Create(DataThread, &this->threadStats[i]);
		}
		Bool running;
		while (true)
		{
			running = true;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (!this->threadStats[i].threadRunning)
				{
					running = false;
					break;
				}
			}
			if (running)
				break;
			this->ctrlEvt->Wait(10);
		}
	}
}

Net::IPScanDetector::~IPScanDetector()
{
	UOSInt i;
	NN<Socket> soc;
	if (this->threadStats)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStats[i].toStop = true;
		}
	}
	if (this->soc.SetTo(soc))
	{
		this->sockf->DestroySocket(soc);
	}
	if (this->threadStats)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStats[i].evt->Set();
		}

		Bool threadRunning = true;
		while (threadRunning)
		{
			threadRunning = false;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->threadStats[i].threadRunning)
				{
					threadRunning = true;
					break;
				}
			}
			if (!threadRunning)
				break;
			this->ctrlEvt->Wait(10);
		}

		i = this->threadCnt;
		while (i-- > 0)
		{
			DEL_CLASS(this->threadStats[i].evt);
		}
		MemFree(this->threadStats);
	}
	this->soc = 0;

	NN<Net::IPScanDetector::AdapterStatus> adapter;
	i = this->adapterMap.GetCount();
	while (i-- > 0)
	{
		adapter = this->adapterMap.GetItemNoCheck(i);
		adapter.Delete();
	}
	SDEL_CLASS(this->ctrlEvt);
}

Bool Net::IPScanDetector::IsError()
{
	return this->soc.IsNull();
}
