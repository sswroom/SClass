#include "Stdafx.h"
#include "Net/TCPPortScanner.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Net::TCPPortScanner::ScanThread(AnyType userObj)
{
	NN<Net::TCPPortScanner> me = userObj.GetNN<Net::TCPPortScanner>();
	Net::SocketUtil::AddressInfo addr;
	OSInt i;
	Socket *s;
	Sync::Interlocked::IncrementUOS(me->threadCnt);
	while (!me->threadToStop)
	{
		me->threadEvt.Wait(10000);
		Sync::MutexUsage mutUsage(me->portMut);
		addr = me->addr;
		if (addr.addrType != Net::AddrType::Unknown)
		{
			i = 0;
			while (i < 65536)
			{
				if (me->portList[i] & 1)
				{
					me->portList[i] = 0;
					mutUsage.EndUse();
					s = 0;
					if (addr.addrType == Net::AddrType::IPv4)
					{
						s = me->sockf->CreateTCPSocketv4();
					}
					else if (addr.addrType == Net::AddrType::IPv6)
					{
						s = me->sockf->CreateTCPSocketv6();
					}
					if (s)
					{
						Bool succ = me->sockf->Connect(s, addr, (UInt16)i, 10000);
						me->sockf->DestroySocket(s);

						mutUsage.BeginUse();
						if (succ)
						{
							if (me->portList[i] == 0)
							{
								me->portList[i] = 2;
								if (me->hdlr.func)
								{
									mutUsage.EndUse();
									me->hdlr.func(me->hdlr.userObj, (UInt16)i);
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
	Sync::Interlocked::DecrementUOS(me->threadCnt);
	return 0;
}

Net::TCPPortScanner::TCPPortScanner(NN<Net::SocketFactory> sockf, UOSInt threadCnt, PortUpdatedHandler hdlr, AnyType userObj)
{
	this->sockf = sockf;
	this->portList = MemAlloc(UInt8, 65536);
	MemClear(this->portList, 65536);
	this->addr.addrType = Net::AddrType::Unknown;
	this->hdlr = {hdlr, userObj};
	this->threadCnt = 0;
	this->threadToStop = false;
	UOSInt i = threadCnt;
	if (threadCnt <= 0)
	{
		i = Sync::ThreadUtil::GetThreadCnt();
	}
	while (i-- > 0)
	{
		Sync::ThreadUtil::Create(ScanThread, this);
	}
}

Net::TCPPortScanner::~TCPPortScanner()
{
	this->threadToStop = true;
	this->threadEvt.Set();
	while (this->threadCnt > 0)
	{
		Sync::SimpleThread::Sleep(1);
	}
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
	this->threadEvt.Set();
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
