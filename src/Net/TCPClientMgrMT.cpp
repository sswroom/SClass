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
#include "Sync/Thread.h"
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
	Data::DateTime *currTime;
	Sync::Event *cliEvt = (Sync::Event*)me->clsData;
	OSInt i;
	Net::TCPClientMgr::TCPClientStatus *cliStat;
	me->clientThreadRunning = true;
	NEW_CLASS(currTime, Data::DateTime());
	while (!me->toStop)
	{
		currTime->SetCurrTimeUTC();
		me->cliMut->Lock();
		i = me->cliArr->GetCount();
		while (i-- > 0)
		{
			cliStat = me->cliArr->GetItem(i);
			if (currTime->DiffMS(cliStat->lastDataTime) > me->timeOutSeconds * 1000)
			{
				cliStat->cli->ShutdownSend();
				cliStat->cli->Close();
			}
			
		}
		me->cliMut->Unlock();
		cliEvt->Wait(1000);
	}
	DEL_CLASS(currTime);
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
			tdata->me->cliMut->Lock();
			i = tdata->me->cliIdArr->SortedIndexOf(tdata->cliStat->cli->GetCliId());
			tdata->me->cliArr->RemoveAt(i);
			tdata->me->cliIdArr->RemoveAt(i);
			tdata->me->cliMut->Unlock();

			DEL_CLASS(tdata->cliStat->lastDataTime);
			DEL_CLASS(tdata->cliStat->readMut);
			MemFree(tdata->cliStat->buff);
			tdata->me->evtHdlr(tdata->cliStat->cli, tdata->me->userObj, tdata->cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_DISCONNECT);
			MemFree(tdata->cliStat);
			break;
		}
		else
		{
			tdata->cliStat->lastDataTime->SetCurrTimeUTC();
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

Net::TCPClientMgr::TCPClientMgr(Int32 timeOutSeconds, TCPClientEvent evtHdlr, TCPClientData dataHdlr, void *userObj, OSInt workerCnt)
{
	this->timeOutSeconds = timeOutSeconds;
	this->evtHdlr = evtHdlr;
	this->dataHdlr = dataHdlr;
	this->userObj = userObj;
	this->toStop = false;
	this->clientThreadRunning = false;
	this->workerCnt = 0;
	NEW_CLASS(this->cliArr, Data::ArrayList<Net::TCPClientMgr::TCPClientStatus*>());
	NEW_CLASS(this->cliIdArr, Data::ArrayListInt64());
	NEW_CLASS(this->cliMut, Sync::Mutex());
	this->workerTasks = 0;
	this->clsData = 0;
	this->workers = 0;
	Sync::Event *cliEvt;
	NEW_CLASS(cliEvt, Sync::Event(true, (const UTF8Char*)"Net.TCPClientMgr.cliEvt"));
	this->clsData = cliEvt;
	Sync::Thread::Create(ClientThread, this);
}

Net::TCPClientMgr::~TCPClientMgr()
{
	Sync::Event *cliEvt = (Sync::Event*)this->clsData;
	this->CloseAll();
	while (cliArr->GetCount() > 0)
	{
		Sync::Thread::Sleep(10);
	}
	this->toStop = true;
	cliEvt->Set();
	while (this->clientThreadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->cliMut);
	DEL_CLASS(this->cliIdArr);
	DEL_CLASS(this->cliArr);
	DEL_CLASS(cliEvt);
}

void Net::TCPClientMgr::AddClient(TCPClient *cli, void *cliData)
{
	this->cliMut->Lock();
	Net::TCPClientMgr::TCPClientStatus *cliStat = MemAlloc(Net::TCPClientMgr::TCPClientStatus, 1);
	cliStat->cli = cli;
	cliStat->cliData = cliData;
	cliStat->buff = MemAlloc(UInt8, TCP_BUFF_SIZE);
	NEW_CLASS(cliStat->lastDataTime, Data::DateTime());
	NEW_CLASS(cliStat->readMut, Sync::Mutex());
	cliStat->lastDataTime->SetCurrTimeUTC();
	cliStat->readMut->Lock();
	cliStat->reading = true;
	this->cliArr->Insert(this->cliIdArr->SortedInsert(cli->GetCliId()), cliStat);
	cliStat->readMut->Unlock();
	this->cliMut->Unlock();
	ThreadData *tdata = MemAlloc(ThreadData, 1);
	tdata->me = this;
	tdata->cliStat = cliStat;
	Sync::Thread::Create(WorkerThread, tdata, 65536);
}

Bool Net::TCPClientMgr::SendClientData(Int64 cliId, const UInt8 *buff, OSInt buffSize)
{
	OSInt i;
	Net::TCPClientMgr::TCPClientStatus *cliStat = 0;
	this->cliMut->Lock();
	i = this->cliIdArr->SortedIndexOf(cliId);
	if (i >= 0)
	{
		cliStat = this->cliArr->GetItem(i);
	}
	this->cliMut->Unlock();
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
	OSInt i = this->cliArr->GetCount();
	this->cliMut->Lock();
	while (i-- > 0)
	{
		Net::TCPClientMgr::TCPClientStatus *cliStat;
		cliStat = this->cliArr->GetItem(i);
		if (cliStat)
		{
			cliStat->cli->Close();
		}
	}
	this->cliMut->Unlock();
}

void Net::TCPClientMgr::BeginGetClient()
{
	this->cliMut->Lock();
}

void Net::TCPClientMgr::EndGetClient()
{
	this->cliMut->Unlock();
}

OSInt Net::TCPClientMgr::GetClientCount()
{
	return this->cliArr->GetCount();
}

Net::TCPClient *Net::TCPClientMgr::GetClient(OSInt index, void **cliData)
{
	Net::TCPClientMgr::TCPClientStatus *cliStat = this->cliArr->GetItem(index);
	if (cliStat)
	{
		*cliData = cliStat->cliData;
		return cliStat->cli;
	}
	return 0;
}

