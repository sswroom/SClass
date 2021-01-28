#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/SocketMonitor.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

UInt32 __stdcall Net::SocketMonitor::DataThread(void *obj)
{
	Net::SocketMonitor::ThreadStat *stat = (Net::SocketMonitor::ThreadStat*)obj;
	stat->threadRunning = true;
	stat->me->ctrlEvt->Set();

	UInt8 *buff = MemAlloc(UInt8, 65536);
	while (!stat->toStop)
	{
		OSInt recvSize;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;
		Net::SocketFactory::ErrorType et;
		recvSize = stat->me->sockf->UDPReceive(stat->me->soc, buff, 65536, &addr, &port, &et);
		Data::DateTime logTime;
		logTime.SetCurrTimeUTC();
		if (recvSize > 0)
		{
			if (stat->me->hdlr)
			{
				stat->me->hdlr(stat->me->userData, buff, recvSize);
			}
		}
	}
	MemFree(buff);
	stat->threadRunning = false;
	stat->me->ctrlEvt->Set();
	return 0;
}

Net::SocketMonitor::SocketMonitor(Net::SocketFactory *sockf, UInt32 *soc, RAWDataHdlr hdlr, void *userData, UOSInt threadCnt)
{
	this->threadCnt = threadCnt;
	this->threadStats = 0;
	UOSInt i;

	this->sockf = sockf;
	this->hdlr = hdlr;
	this->userData = userData;
	this->ctrlEvt = 0;

	this->soc = soc;
	if (this->soc)
	{
		NEW_CLASS(this->ctrlEvt, Sync::Event(true, (const UTF8Char*)"Net.UDPServer.ctrlEvt"));

		this->threadStats = MemAlloc(ThreadStat, this->threadCnt);
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStats[i].toStop = false;
			this->threadStats[i].threadRunning = false;
			NEW_CLASS(this->threadStats[i].evt, Sync::Event(true, (const UTF8Char*)"Net.UDPServer.threadEvt"));
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

Net::SocketMonitor::~SocketMonitor()
{
	UOSInt i;
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
		MemFree(this->threadStats);
	}
	if (this->soc)
	{
		this->soc = 0;
	}

	SDEL_CLASS(this->ctrlEvt);
}
