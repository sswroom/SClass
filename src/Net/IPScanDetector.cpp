#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Net/IPScanDetector.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

#define TIMEDETECTRANGE 3000
#define DETECTCOUNT 5

UInt32 __stdcall Net::IPScanDetector::DataThread(void *obj)
{
	Net::IPScanDetector::ThreadStat *stat = (Net::IPScanDetector::ThreadStat*)obj;
	UInt8 macBuff[8];
	stat->threadRunning = true;
	stat->me->ctrlEvt->Set();

	UInt8 *buff = MemAlloc(UInt8, 2048);
	while (!stat->toStop)
	{
		OSInt recvSize;
		Net::SocketUtil::AddressInfo recvAddr;
		UInt16 recvPort;

		recvSize = stat->me->sockf->UDPReceive(stat->me->soc, buff, 2048, &recvAddr, &recvPort, 0);
		if (recvSize >= 42)
		{
			UInt16 opcode = ReadMUInt16(&buff[20]);
			if (opcode == 1) //Request
			{
				Int32 ip;
				Int64 reqTime;
				Int64 lastTime;
				Int64 iMAC;
				Net::IPScanDetector::AdapterStatus *adapter;
				Data::DateTime dt;
				dt.SetCurrTimeUTC();
				reqTime = dt.ToTicks();
				macBuff[0] = 0;
				macBuff[1] = 0;
				MemCopyNO(&macBuff[2], &buff[6], 6);
				iMAC = ReadMInt64(macBuff);
				ip = ReadMInt32(&buff[38]);
				stat->me->adapterMut->Lock();
				adapter = stat->me->adapterMap->Get(iMAC);
				if (adapter)
				{
					if (adapter->lastDetectTime + TIMEDETECTRANGE < reqTime)
					{
						adapter->detectCnt = 1;
					}
					else
					{
						lastTime = adapter->targetIPMap->Get(ip);
						if (lastTime == 0 || lastTime + TIMEDETECTRANGE < reqTime)
						{
							adapter->detectCnt++;
							if (adapter->detectCnt == DETECTCOUNT)
							{
								stat->me->hdlr(&macBuff[2], stat->me->userData);
							}
						}
					}
					adapter->targetIPMap->Put(ip, reqTime);
					adapter->lastDetectTime = reqTime;
				}
				else
				{
					adapter = MemAlloc(Net::IPScanDetector::AdapterStatus, 1);
					adapter->iMAC = iMAC;
					adapter->lastDetectTime = reqTime;
					adapter->detectCnt = 1;
					NEW_CLASS(adapter->targetIPMap, Data::Integer32Map<Int64>());
					adapter->targetIPMap->Put(ip, reqTime);
					stat->me->adapterMap->Put(iMAC, adapter);
				}
				stat->me->adapterMut->Unlock();
			}
			else if (opcode == 2) //Reply
			{
				Int32 ip;
				Int64 reqTime;
				Int64 lastTime;
				Int64 iMAC;
				Net::IPScanDetector::AdapterStatus *adapter;
				Data::DateTime dt;
				dt.SetCurrTimeUTC();
				reqTime = dt.ToTicks();
				macBuff[0] = 0;
				macBuff[1] = 0;
				MemCopyNO(&macBuff[2], &buff[0], 6);
				iMAC = ReadMInt64(macBuff);
				ip = ReadMInt32(&buff[28]);
				stat->me->adapterMut->Lock();
				adapter = stat->me->adapterMap->Get(iMAC);
				if (adapter)
				{
					if (adapter->lastDetectTime + TIMEDETECTRANGE < reqTime)
					{
						adapter->detectCnt = 1;
					}
					else
					{
						lastTime = adapter->targetIPMap->Get(ip);
						if (lastTime == 0 || lastTime + TIMEDETECTRANGE < reqTime)
						{
							adapter->detectCnt++;
							if (adapter->detectCnt == DETECTCOUNT)
							{
								stat->me->hdlr(&macBuff[2], stat->me->userData);
							}
						}
					}
					adapter->targetIPMap->Put(ip, reqTime);
					adapter->lastDetectTime = reqTime;
				}
				else
				{
					adapter = MemAlloc(Net::IPScanDetector::AdapterStatus, 1);
					adapter->iMAC = iMAC;
					adapter->lastDetectTime = reqTime;
					adapter->detectCnt = 1;
					NEW_CLASS(adapter->targetIPMap, Data::Integer32Map<Int64>());
					adapter->targetIPMap->Put(ip, reqTime);
					stat->me->adapterMap->Put(iMAC, adapter);
				}
				stat->me->adapterMut->Unlock();
			}
		}
	}
	MemFree(buff);
	stat->threadRunning = false;
	stat->me->ctrlEvt->Set();
	return 0;
}

Net::IPScanDetector::IPScanDetector(Net::SocketFactory *sockf, IPScanHandler hdlr, void *userData, OSInt threadCnt)
{
	OSInt i;
	this->threadCnt = threadCnt;
	this->sockf = sockf;
	this->hdlr = hdlr;
	this->userData = userData;
	this->ctrlEvt = 0;
	NEW_CLASS(this->adapterMap, Data::Integer64Map<Net::IPScanDetector::AdapterStatus*>());
	NEW_CLASS(this->adapterMut, Sync::Mutex());
	this->soc = this->sockf->CreateARPSocket();

	if (this->soc)
	{
		NEW_CLASS(this->ctrlEvt, Sync::Event(true, (const UTF8Char*)"Net.IPScanDetector.ctrlEvt"));

		this->threadStats = MemAlloc(Net::IPScanDetector::ThreadStat, this->threadCnt);

		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStats[i].toStop = false;
			this->threadStats[i].threadRunning = false;
			NEW_CLASS(this->threadStats[i].evt, Sync::Event(true, (const UTF8Char*)"Net.IPScanDetector.threadEvt"));
			this->threadStats[i].me = this;
			Sync::Thread::Create(DataThread, &this->threadStats[i]);
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
	OSInt i;
	if (this->threadStats)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStats[i].toStop = true;
		}
	}
	if (this->soc)
	{
		this->sockf->DestroySocket(this->soc);
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
	if (this->soc)
	{
		this->sockf->DestroySocket(this->soc);
		this->soc = 0;
	}

	Data::ArrayList<Net::IPScanDetector::AdapterStatus*> *adapterList = this->adapterMap->GetValues();
	Net::IPScanDetector::AdapterStatus *adapter;
	i = adapterList->GetCount();
	while (i-- > 0)
	{
		adapter = adapterList->GetItem(i);
		DEL_CLASS(adapter->targetIPMap);
		MemFree(adapter);
	}
	DEL_CLASS(this->adapterMut);
	SDEL_CLASS(this->ctrlEvt);
}

Bool Net::IPScanDetector::IsError()
{
	return this->soc == 0;
}
