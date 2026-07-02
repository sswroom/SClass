#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
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
	NN<Sync::Event> ctrlEvt;
	stat->threadRunning = true;
	if (stat->me->ctrlEvt.SetTo(ctrlEvt)) ctrlEvt->Set();
	NN<Socket> soc;
	if (stat->me->soc.SetTo(soc))
	{
		UnsafeArray<UInt8> buff = MemAllocArr(UInt8, 2048);
		while (!stat->toStop)
		{
			UIntOS recvSize;
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
		MemFreeArr(buff);
	}
	stat->threadRunning = false;
	if (stat->me->ctrlEvt.SetTo(ctrlEvt)) ctrlEvt->Set();
	return 0;
}

Net::IPScanDetector::IPScanDetector(NN<Net::SocketFactory> sockf, IPScanHandler hdlr, AnyType userData, UIntOS threadCnt)
{
	UIntOS i;
	this->threadCnt = threadCnt;
	this->sockf = sockf;
	this->hdlr = hdlr;
	this->userData = userData;
	this->ctrlEvt = nullptr;
	this->soc = this->sockf->CreateARPSocket();
	this->threadStats = nullptr;

	UnsafeArray<ThreadStat> threadStats;
	NN<Socket> soc;
	if (this->soc.SetTo(soc))
	{
		NN<Sync::Event> ctrlEvt;
		NEW_CLASSNN(ctrlEvt, Sync::Event(true));
		this->ctrlEvt = ctrlEvt;

		this->threadStats = threadStats = MemAllocArr(Net::IPScanDetector::ThreadStat, this->threadCnt);

		i = this->threadCnt;
		while (i-- > 0)
		{
			threadStats[i].toStop = false;
			threadStats[i].threadRunning = false;
			NEW_CLASSNN(threadStats[i].evt, Sync::Event(true));
			threadStats[i].me = *this;
			Sync::ThreadUtil::Create(DataThread, &threadStats[i]);
		}
		Bool running;
		while (true)
		{
			running = true;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (!threadStats[i].threadRunning)
				{
					running = false;
					break;
				}
			}
			if (running)
				break;
			ctrlEvt->Wait(10);
		}
	}
}

Net::IPScanDetector::~IPScanDetector()
{
	UIntOS i;
	NN<Socket> soc;
	NN<Sync::Event> ctrlEvt;
	UnsafeArray<ThreadStat> threadStats;
	if (this->threadStats.SetTo(threadStats))
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			threadStats[i].toStop = true;
		}
	}
	if (this->soc.SetTo(soc))
	{
		this->sockf->DestroySocket(soc);
	}
	if (this->threadStats.SetTo(threadStats))
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			threadStats[i].evt->Set();
		}

		Bool threadRunning = true;
		while (threadRunning)
		{
			threadRunning = false;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (threadStats[i].threadRunning)
				{
					threadRunning = true;
					break;
				}
			}
			if (!threadRunning)
				break;
			if (this->ctrlEvt.SetTo(ctrlEvt)) ctrlEvt->Wait(10);
		}

		i = this->threadCnt;
		while (i-- > 0)
		{
			threadStats[i].evt.Delete();
		}
		MemFreeArr(threadStats);
	}
	this->soc = nullptr;

	NN<Net::IPScanDetector::AdapterStatus> adapter;
	i = this->adapterMap.GetCount();
	while (i-- > 0)
	{
		adapter = this->adapterMap.GetItemNoCheck(i);
		adapter.Delete();
	}
	this->ctrlEvt.Delete();
}

Bool Net::IPScanDetector::IsError()
{
	return this->soc.IsNull();
}
