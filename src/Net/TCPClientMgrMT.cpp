#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "Data/ArrayList.h"
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
	Net::TCPClientMgr *me;
	Net::TCPClientMgr::TCPClientStatus *cliStat;
} ThreadData;

UInt32 __stdcall Net::TCPClientMgr::ClientThread(void *o)
{
	Net::TCPClientMgr *me = (Net::TCPClientMgr*)o;
	Data::Timestamp currTime;
	Sync::Event *cliEvt = (Sync::Event*)me->clsData;
	OSInt i;
	Net::TCPClientMgr::TCPClientStatus *cliStat;
	Sync::ThreadUtil::SetName((const UTF8Char*)"TCPCliMgr");
	me->clientThreadRunning = true;
	while (!me->toStop)
	{
		currTime = Data::Timestamp::UtcNow();
		Sync::MutexUsage mutUsage(me->cliMut);
		i = me->cliArr.GetCount();
		while (i-- > 0)
		{
			cliStat = me->cliArr.GetItem(i);
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

UInt32 __stdcall Net::TCPClientMgr::WorkerThread(void *o)
{
	ThreadData *tdata = (ThreadData*)o;
	OSInt i;
	while (true)
	{
		OSInt readSize = tdata->cliStat->cli->Read(tdata->cliStat->buff, TCP_BUFF_SIZE);
		if (readSize <= 0)
		{
			Sync::MutexUsage mutUsage(tdata->me->cliMut);
			i = tdata->me->cliIdArr.SortedIndexOf(tdata->cliStat->cli->GetCliId());
			tdata->me->cliArr.RemoveAt(i);
			tdata->me->cliIdArr.RemoveAt(i);
			mutUsage.EndUse();

			tdata->me->evtHdlr(tdata->cliStat->cli, tdata->me->userObj, tdata->cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_DISCONNECT);
			DEL_CLASS(tdata->cliStat);
			break;
		}
		else
		{
			tdata->cliStat->lastDataTime = Data::Timestamp::UtcNow();
			tdata->me->evtHdlr(tdata->cliStat->cli, tdata->me->userObj, tdata->cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_HASDATA);
			tdata->me->dataHdlr(tdata->cliStat->cli, tdata->me->userObj, tdata->cliStat->cliData, tdata->cliStat->buff, readSize);
		}
	}
	MemFree(tdata);
	return 0;
}

void Net::TCPClientMgr::ProcessClient(Net::TCPClientMgr::TCPClientStatus *cliStat)
{
}

Net::TCPClientMgr::TCPClientMgr(Int32 timeOutSeconds, TCPClientEvent evtHdlr, TCPClientData dataHdlr, void *userObj, UOSInt workerCnt, TCPClientTimeout toHdlr)
{
	this->timeout = Data::Duration(timeOutSeconds, 0);
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
	Sync::Event *cliEvt = (Sync::Event*)this->clsData;
	this->CloseAll();
	while (this->cliArr.GetCount() > 0)
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

void Net::TCPClientMgr::AddClient(TCPClient *cli, void *cliData)
{
	cli->SetNoDelay(true);
	cli->SetTimeout(this->timeout);
	Sync::MutexUsage mutUsage(this->cliMut);
	Net::TCPClientMgr::TCPClientStatus *cliStat;
	NEW_CLASS(cliStat, Net::TCPClientMgr::TCPClientStatus());
	cliStat->cli = cli;
	cliStat->cliData = cliData;
	cliStat->lastDataTime = Data::Timestamp::UtcNow();
	Sync::MutexUsage readMutUsage(cliStat->readMut);
	cliStat->reading = true;
	this->cliArr.Insert(this->cliIdArr.SortedInsert(cli->GetCliId()), cliStat);
	readMutUsage.EndUse();
	mutUsage.EndUse();
	ThreadData *tdata = MemAlloc(ThreadData, 1);
	tdata->me = this;
	tdata->cliStat = cliStat;
	Sync::ThreadUtil::Create(WorkerThread, tdata, 65536);
}

Bool Net::TCPClientMgr::SendClientData(UInt64 cliId, const UInt8 *buff, UOSInt buffSize)
{
	OSInt i;
	Net::TCPClientMgr::TCPClientStatus *cliStat = 0;
	Sync::MutexUsage mutUsage(this->cliMut);
	i = this->cliIdArr.SortedIndexOf(cliId);
	if (i >= 0)
	{
		cliStat = this->cliArr.GetItem(i);
	}
	mutUsage.EndUse();
	if (cliStat)
	{
		return cliStat->cli->Write(buff, buffSize) == buffSize;
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
	OSInt i = this->cliArr.GetCount();
	Sync::MutexUsage mutUsage(this->cliMut);
	while (i-- > 0)
	{
		Net::TCPClientMgr::TCPClientStatus *cliStat;
		cliStat = this->cliArr.GetItem(i);
		if (cliStat)
		{
			cliStat->cli->Close();
		}
	}
	mutUsage.EndUse();
}

void Net::TCPClientMgr::UseGetClient(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->cliMut);
}

UOSInt Net::TCPClientMgr::GetClientCount() const
{
	return this->cliArr.GetCount();
}

void Net::TCPClientMgr::ExtendTimeout(NotNullPtr<Net::TCPClient> cli)
{
	Sync::MutexUsage mutUsage(this->cliMut);
	OSInt i = this->cliIdArr.SortedIndexOf(cli->GetCliId());
	if (i >= 0)
	{
		Net::TCPClientMgr::TCPClientStatus *cliStat = this->cliArr.GetItem(i);
		cliStat->lastDataTime = Data::Timestamp::UtcNow();
	}
}

Net::TCPClient *Net::TCPClientMgr::GetClient(UOSInt index, void **cliData)
{
	Net::TCPClientMgr::TCPClientStatus *cliStat = this->cliArr.GetItem(index);
	if (cliStat)
	{
		*cliData = cliStat->cliData;
		return cliStat->cli;
	}
	return 0;
}

