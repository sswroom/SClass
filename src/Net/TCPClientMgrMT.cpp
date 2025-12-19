#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Data/ArrayList.hpp"
#include "IO/Stream.h"
#include "IO/Console.h"
#include "Manage/HiResClock.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Net/TCPClientMgr.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderW.h"

#define TCP_BUFF_SIZE 2048

typedef struct
{
	NN<Net::TCPClientMgr> me;
	NN<Net::TCPClientMgr::TCPClientStatus> cliStat;
} ThreadData;

UInt32 __stdcall Net::TCPClientMgr::ClientThread(AnyType o)
{
	NN<Net::TCPClientMgr> me = o.GetNN<Net::TCPClientMgr>();
	Data::Timestamp currTime;
	Sync::Event *cliEvt = (Sync::Event*)me->clsData.OrNull();
	UOSInt i;
	NN<Net::TCPClientMgr::TCPClientStatus> cliStat;
	Sync::ThreadUtil::SetName(CSTR("TCPCliMgr"));
	me->clientThreadRunning = true;
	while (!me->toStop)
	{
		currTime = Data::Timestamp::UtcNow();
		Sync::MutexUsage mutUsage(me->cliMut);
		i = me->cliMap.GetCount();
		while (i-- > 0)
		{
			cliStat = me->cliMap.GetItemNoCheck(i);
			if ((currTime - cliStat->lastDataTime) > cliStat->cli->GetTimeout())
			{
				cliStat->cli->ShutdownSend();
				cliStat->cli->Close();
			}
			
		}
		mutUsage.EndUse();
		cliEvt->Wait(1000);
	}
	me->clientThreadRunning = false;
	return 0;
}

UInt32 __stdcall Net::TCPClientMgr::WorkerThread(AnyType o)
{
	NN<ThreadData> tdata = o.GetNN<ThreadData>();
	UOSInt i;
	while (true)
	{
		UOSInt readSize = tdata->cliStat->cli->Read(Data::ByteArray(tdata->cliStat->buff, TCP_BUFF_SIZE));
		if (readSize <= 0)
		{
			Sync::MutexUsage mutUsage(tdata->me->cliMut);
			tdata->me->cliMap.Remove(tdata->cliStat->cli->GetCliId());
			mutUsage.EndUse();

			tdata->me->evtHdlr(tdata->cliStat->cli, tdata->me->userObj, tdata->cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_DISCONNECT);
			tdata->cliStat.Delete();
			break;
		}
		else
		{
			tdata->cliStat->lastDataTime = Data::Timestamp::UtcNow();
			tdata->me->evtHdlr(tdata->cliStat->cli, tdata->me->userObj, tdata->cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_HASDATA);
			tdata->me->dataHdlr(tdata->cliStat->cli, tdata->me->userObj, tdata->cliStat->cliData, Data::ByteArrayR(tdata->cliStat->buff, readSize));
		}
	}
	MemFreeNN(tdata);
	return 0;
}

void Net::TCPClientMgr::ProcessClient(NN<Net::TCPClientMgr::TCPClientStatus> cliStat)
{
}

Net::TCPClientMgr::TCPClientMgr(Int32 timeOutSeconds, TCPClientEvent evtHdlr, TCPClientData dataHdlr, AnyType userObj, UOSInt workerCnt, TCPClientTimeout toHdlr)
{
	this->timeout = Data::Duration::FromSecNS(timeOutSeconds, 0);
	this->evtHdlr = evtHdlr;
	this->dataHdlr = dataHdlr;
	this->toHdlr = toHdlr;
	this->userObj = userObj;
	this->toStop = false;
	this->clientThreadRunning = false;
	this->workerCnt = 0;
	this->clsData = 0;
	this->workers = 0;
	Sync::Event *cliEvt;
	NEW_CLASS(cliEvt, Sync::Event(true));
	this->clsData = (ClassData*)cliEvt;
	Sync::ThreadUtil::Create(ClientThread, this);
}

Net::TCPClientMgr::~TCPClientMgr()
{
	Sync::Event *cliEvt = (Sync::Event*)this->clsData.OrNull();
	this->CloseAll();
	while (this->cliMap.GetCount() > 0)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->toStop = true;
	cliEvt->Set();
	while (this->clientThreadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	DEL_CLASS(cliEvt);
}

void Net::TCPClientMgr::AddClient(NN<TCPClient> cli, AnyType cliData)
{
	cli->SetNoDelay(true);
	cli->SetTimeout(this->timeout);
	Sync::MutexUsage mutUsage(this->cliMut);
	NN<Net::TCPClientMgr::TCPClientStatus> cliStat;
	NEW_CLASSNN(cliStat, Net::TCPClientMgr::TCPClientStatus());
	cliStat->cli = cli;
	cliStat->cliData = cliData;
	cliStat->lastDataTime = Data::Timestamp::UtcNow();
	Sync::MutexUsage readMutUsage(cliStat->readMut);
	cliStat->reading = true;
	this->cliMap.Put(cli->GetCliId(), cliStat);
	readMutUsage.EndUse();
	mutUsage.EndUse();
	NN<ThreadData> tdata = MemAllocNN(ThreadData);
	tdata->me = *this;
	tdata->cliStat = cliStat;
	Sync::ThreadUtil::Create(WorkerThread, tdata, 65536);
}

Bool Net::TCPClientMgr::SendClientData(UInt64 cliId, UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	NN<Net::TCPClientMgr::TCPClientStatus> cliStat;
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cliMap.Get(cliId).SetTo(cliStat))
	{
		mutUsage.EndUse();
		return cliStat->cli->Write(Data::ByteArrayR(buff, buffSize)) == buffSize;
	}
	else
	{
		return false;
	}
}

Bool Net::TCPClientMgr::IsError()
{
	return false;
}

void Net::TCPClientMgr::CloseAll()
{
	Sync::MutexUsage mutUsage(this->cliMut);
	UOSInt i = this->cliMap.GetCount();
	while (i-- > 0)
	{
		NN<Net::TCPClientMgr::TCPClientStatus> cliStat;
		if (this->cliMap.GetItem(i).SetTo(cliStat))
		{
			cliStat->cli->Close();
		}
	}
}

void Net::TCPClientMgr::UseGetClient(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->cliMut);
}

UOSInt Net::TCPClientMgr::GetClientCount() const
{
	return this->cliMap.GetCount();
}

void Net::TCPClientMgr::ExtendTimeout(NN<Net::TCPClient> cli)
{
	Sync::MutexUsage mutUsage(this->cliMut);
	NN<Net::TCPClientMgr::TCPClientStatus> cliStat;
	if (this->cliMap.Get(cli->GetCliId()).SetTo(cliStat))
	{
		cliStat->lastDataTime = Data::Timestamp::UtcNow();
	}
}

Optional<Net::TCPClient> Net::TCPClientMgr::GetClient(UOSInt index, OutParam<AnyType> cliData)
{
	NN<Net::TCPClientMgr::TCPClientStatus> cliStat;
	if (this->cliMap.GetItem(index).SetTo(cliStat))
	{
		cliData.Set(cliStat->cliData);
		return cliStat->cli;
	}
	return 0;
}

