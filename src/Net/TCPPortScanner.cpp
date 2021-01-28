#include "Stdafx.h"
#include "Net/TCPPortScanner.h"
#include "Sync/Interlocked.h"
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
		me->portMut->Lock();
		addr = me->addr;
		if (addr.addrType != Net::SocketUtil::AT_UNKNOWN)
		{
			i = 0;
			while (i < 65536)
			{
				if (me->portList[i] & 1)
				{
					me->portList[i] = 0;
					me->portMut->Unlock();
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

						me->portMut->Lock();
						if (succ)
						{
							if (me->portList[i] == 0)
							{
								me->portList[i] = 2;
								if (me->hdlr)
								{
									me->portMut->Unlock();
									me->hdlr(me->hdlrObj, (UInt16)i);
									me->portMut->Lock();
								}
							}
						}
					}
					else
					{
						me->portMut->Lock();
					}
					if (me->threadToStop)
					{
						break;
					}
				}
				i++;
			}
		}
		me->portMut->Unlock();
	}
	Sync::Interlocked::Decrement(&me->threadCnt);
	return 0;
}

Net::TCPPortScanner::TCPPortScanner(Net::SocketFactory *sockf, OSInt threadCnt, PortUpdatedHandler hdlr, void *userObj)
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
	OSInt i = threadCnt;
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
	this->portMut->Lock();
	this->addr = *addr;
	i = 0;
	while (i < j)
	{
		this->portList[i] = 1;
		i++;
	}
	this->portMut->Unlock();
	this->threadEvt->Set();
}

Bool Net::TCPPortScanner::IsFinished()
{
	Bool ret = true;
	UInt16 i = 0;
	this->portMut->Lock();
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
	this->portMut->Unlock();
	return ret;
}

OSInt Net::TCPPortScanner::GetAvailablePorts(Data::ArrayList<UInt16> *portList)
{
	OSInt initCnt = portList->GetCount();
	UInt16 i = 0;
	this->portMut->Lock();
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
	this->portMut->Unlock();
	return portList->GetCount() - initCnt;
}
