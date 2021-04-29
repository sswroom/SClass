#include "Stdafx.h"
#include "Net/TCPPortScanner.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

UInt32 __stdcall Net::TCPPortScanner::ScanThread(void *userObj)
{
	Net::TCPPortScanner *me = (Net::TCPPortScanner*)userObj;
	Net::SocketUtil::AddressInfo addr;
	OSInt i;
	UInt32 *s;
	Sync::Interlocked::Increment(&me->threadCnt);
	while (!me->threadToStop)
	{
		me->threadEvt->Wait(10000);
		Sync::MutexUsage mutUsage(me->portMut);
		addr = me->addr;
		if (addr.addrType != Net::SocketUtil::AT_UNKNOWN)
		{
			i = 0;
			while (i < 65536)
			{
				if (me->portList[i] & 1)
				{
					me->portList[i] = 0;
					mutUsage.EndUse();
					s = 0;
					if (addr.addrType == Net::SocketUtil::AT_IPV4)
					{
						s = me->sockf->CreateTCPSocketv4();
					}
					else if (addr.addrType == Net::SocketUtil::AT_IPV6)
					{
						s = me->sockf->CreateTCPSocketv6();
					}
					if (s)
					{
						Bool succ = me->sockf->Connect(s, &addr, (UInt16)i);
						me->sockf->DestroySocket(s);

						mutUsage.BeginUse();
						if (succ)
						{
							if (me->portList[i] == 0)
							{
								me->portList[i] = 2;
								if (me->hdlr)
								{
									mutUsage.EndUse();
									me->hdlr(me->hdlrObj, (UInt16)i);
									mutUsage.BeginUse();
								}
							}
						}
					}
					else
					{
						mutUsage.BeginUse();
					}
					if (me->threadToStop)
					{
						break;
					}
				}
				i++;
			}
		}
		mutUsage.EndUse();
	}
	Sync::Interlocked::Decrement((OSInt*)&me->threadCnt);
	return 0;
}

Net::TCPPortScanner::TCPPortScanner(Net::SocketFactory *sockf, UOSInt threadCnt, PortUpdatedHandler hdlr, void *userObj)
{
	this->sockf = sockf;
	this->portList = MemAlloc(UInt8, 65536);
	MemClear(this->portList, 65536);
	NEW_CLASS(this->portMut, Sync::Mutex());
	this->addr.addrType = Net::SocketUtil::AT_UNKNOWN;
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
	this->threadCnt = 0;
	this->threadToStop = false;
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"Net.TCPPortScanner.threadEvt"));
	UOSInt i = threadCnt;
	if (threadCnt <= 0)
	{
		i = Sync::Thread::GetThreadCnt();
	}
	while (i-- > 0)
	{
		Sync::Thread::Create(ScanThread, this);
	}
}

Net::TCPPortScanner::~TCPPortScanner()
{
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadCnt > 0)
	{
		Sync::Thread::Sleep(1);
	}
	DEL_CLASS(this->threadEvt);
	DEL_CLASS(this->portMut);
	MemFree(this->portList);
}

void Net::TCPPortScanner::Start(Net::SocketUtil::AddressInfo *addr, UInt16 maxPort)
{
	OSInt i;
	OSInt j = maxPort + 1;
	Sync::MutexUsage mutUsage(this->portMut);
	this->addr = *addr;
	i = 0;
	while (i < j)
	{
		this->portList[i] = 1;
		i++;
	}
	mutUsage.EndUse();
	this->threadEvt->Set();
}

Bool Net::TCPPortScanner::IsFinished()
{
	Bool ret = true;
	UInt16 i = 0;
	Sync::MutexUsage mutUsage(this->portMut);
	while (i < 65535)
	{
		if ((this->portList[i] & 1) == 1)
		{
			ret = false;
			break;
		}
		i++;
	}
	if ((this->portList[65535] & 1) == 1)
	{
		ret = false;
	}
	mutUsage.EndUse();
	return ret;
}

UOSInt Net::TCPPortScanner::GetAvailablePorts(Data::ArrayList<UInt16> *portList)
{
	UOSInt initCnt = portList->GetCount();
	UInt16 i = 0;
	Sync::MutexUsage mutUsage(this->portMut);
	while (i < 65535)
	{
		if (this->portList[i] == 2)
		{
			portList->Add(i);

		}
		i++;
	}
	if (this->portList[65535] == 2)
	{
		portList->Add(65535);
	}
	mutUsage.EndUse();
	return portList->GetCount() - initCnt;
}
