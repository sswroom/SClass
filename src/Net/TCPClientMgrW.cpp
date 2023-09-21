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

#define PROCESS_TIMEOUT_DURATION 30000

UInt32 __stdcall Net::TCPClientMgr::ClientThread(void *o)
{
	Net::TCPClientMgr *me = (Net::TCPClientMgr*)o;
	Double t;
	UInt32 waitPeriod = 1;
	Data::Timestamp intT;
	Data::Timestamp currTime;
	UOSInt i;
	UOSInt readSize;
	Bool found;
	NotNullPtr<Net::TCPClientMgr::TCPClientStatus> cliStat;
	Sync::ThreadUtil::SetName(CSTR("TCPCliMgr"));
	me->clientThreadRunning = true;
	{
		Manage::HiResClock clk;
		Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_REALTIME);
		while (!me->toStop)
		{
			found = false;
			((Sync::Event*)me->clsData)->Clear();
			clk.Start();
			currTime = Data::Timestamp::UtcNow();
			intT = currTime;
			i = me->cliMap.GetCount();
			while (i-- > 0)
			{
				Sync::MutexUsage cliMutUsage(me->cliMut);
				if (cliStat.Set((Net::TCPClientMgr::TCPClientStatus*)me->cliMap.GetItem(i)))
				{
					if (cliStat->reading)
					{
						Sync::MutexUsage mutUsage(cliStat->readMut);
						Bool incomplete = false;
						if (cliStat->readReq == 0)
						{
							if (cliStat->cli->IsClosed())
							{
								readSize = 0;
							}
							else
							{
								readSize = 0;
							}
						}
						else
						{
							if (cliStat->cli->IsClosed())
							{
								Sync::SimpleThread::Sleep(1);
							}
							readSize = cliStat->cli->EndRead(cliStat->readReq, false, incomplete);
						}
						mutUsage.EndUse();
						if (incomplete)
						{
							currTime = Data::Timestamp::UtcNow();
							if (currTime.Diff(cliStat->lastDataTime) > cliStat->cli->GetTimeout())
							{
								if (me->logWriter) me->logWriter->TCPDisconnect(cliStat->cli);
								cliStat->cli->ShutdownSend();
								cliStat->cli->Close();
								Sync::SimpleThread::Sleep(1);
								cliStat->cli->EndRead(cliStat->readReq, true, incomplete);
								me->cliMap.RemoveAt(i);
								cliMutUsage.EndUse();

								me->evtHdlr(cliStat->cli, me->userObj, cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_DISCONNECT);
								cliStat.Delete();
							}
							else
							{
								cliMutUsage.EndUse();
							}
						}
						else if (readSize)
						{
							cliStat->reading = false;
							cliMutUsage.EndUse();
							if (me->logWriter) me->logWriter->TCPRecv(cliStat->cli, cliStat->buff, readSize);
							cliStat->lastDataTime = Data::Timestamp::UtcNow();
							me->evtHdlr(cliStat->cli, me->userObj, cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_HASDATA);
							cliStat->buffSize = readSize;
							me->ProcessClient(cliStat);
							found = true;
						}
						else
						{
							if (me->logWriter) me->logWriter->TCPDisconnect(cliStat->cli);
							me->cliMap.RemoveAt(i);
							cliMutUsage.EndUse();

							me->evtHdlr(cliStat->cli, me->userObj, cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_DISCONNECT);
							cliStat.Delete();
						}
					}
					else
					{
						if (cliStat->processing && !cliStat->timeAlerted && intT.Diff(cliStat->timeStart).GetTotalMS() >= PROCESS_TIMEOUT_DURATION)
						{
							cliStat->timeAlerted = true;
							if (me->toHdlr)
							{
								me->toHdlr(cliStat->cli, me->userObj, cliStat->cliData);
							}
						}
						cliMutUsage.EndUse();
					}
				}
				else
				{
					cliMutUsage.EndUse();
				}
			}
			t = clk.GetTimeDiff();
			if (t > 0.1)
			{
				clk.Start();
			}

			if (!found)
			{
				clk.Start();
				((Sync::Event*)me->clsData)->Wait(waitPeriod);
				t = clk.GetTimeDiff();
				if (t > 0.5 && ((Sync::Event*)me->clsData)->IsSet())
				{
					clk.Start();
				}
				waitPeriod = waitPeriod << 1;
				if (waitPeriod >= 800)
				{
					waitPeriod = 800;
				}
			}
			else
			{
				waitPeriod = 1;
			}
		}
	}
	me->clientThreadRunning = false;
	return 0;
}

UInt32 __stdcall Net::TCPClientMgr::WorkerThread(void *o)
{
	Net::TCPClientMgr::WorkerStatus *stat = (Net::TCPClientMgr::WorkerStatus*)o;
	Net::TCPClientMgr *me = stat->me;
	NotNullPtr<Net::TCPClientMgr::TCPClientStatus> cliStat;
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("TCPCliMgr")), stat->index);
	Sync::ThreadUtil::SetName(CSTRP(sbuff, sptr));
	stat->state = WorkerState::Idle;
	while (!stat->toStop)
	{
		while (cliStat.Set(me->workerTasks.Get()))
		{
			stat->state = WorkerState::Processing;
			cliStat->timeStart = Data::Timestamp::UtcNow();
			cliStat->timeAlerted = false;
			cliStat->processing = true;
			me->dataHdlr(cliStat->cli, me->userObj, cliStat->cliData, Data::ByteArrayR(cliStat->buff, cliStat->buffSize));
			cliStat->processing = false;
			me->ClientBeginRead(cliStat);
		}
		stat->state = WorkerState::Idle;
		stat->evt->Wait(7700);
	}
	stat->state = WorkerState::Stopped;
	return 0;
}

void Net::TCPClientMgr::ProcessClient(NotNullPtr<Net::TCPClientMgr::TCPClientStatus> cliStat)
{
	this->workerTasks.Put(cliStat.Ptr());
	UOSInt i = this->workerCnt;
	while (i-- > 0)
	{
		if (this->workers[i].state == WorkerState::Idle)
		{
			this->workers[i].evt->Set();
			return;
		}
	}
}

void Net::TCPClientMgr::ClientBeginRead(NotNullPtr<TCPClientStatus> cliStat)
{
	Bool setEvt = false;
	if (cliStat->cli->GetRecvBuffSize() > 0)
	{
		setEvt = true;
	}

	Sync::MutexUsage mutUsage(cliStat->readMut);
	cliStat->readReq = cliStat->cli->BeginRead(Data::ByteArray(cliStat->buff, TCP_BUFF_SIZE), ((Sync::Event*)this->clsData));
	cliStat->reading = true;
	mutUsage.EndUse();
	if (setEvt)
	{
		((Sync::Event*)this->clsData)->Set();
	}
}

Net::TCPClientMgr::TCPClientMgr(Int32 timeOutSeconds, TCPClientEvent evtHdlr, TCPClientData dataHdlr, void *userObj, UOSInt workerCnt, TCPClientTimeout toHdlr)
{
	this->timeout = Data::Duration(timeOutSeconds, 0);
	this->logWriter = 0;
	this->evtHdlr = evtHdlr;
	this->dataHdlr = dataHdlr;
	this->toHdlr = toHdlr;
	this->userObj = userObj;
	this->toStop = false;
	this->clientThreadRunning = false;
	if (workerCnt <= 0)
		workerCnt = 1;
	this->workerCnt = workerCnt;
	Sync::Event *recvEvt;
	NEW_CLASS(recvEvt, Sync::Event(false));
	this->clsData = (ClassData*)recvEvt;
	Sync::ThreadUtil::Create(ClientThread, this);
	this->workers = MemAlloc(WorkerStatus, workerCnt);
	while (workerCnt-- > 0)
	{
		this->workers[workerCnt].index = workerCnt;
		this->workers[workerCnt].state = WorkerState::NotStarted;
		this->workers[workerCnt].toStop = false;
		this->workers[workerCnt].isPrimary = (workerCnt == 0);
		this->workers[workerCnt].me = this;
		NEW_CLASS(this->workers[workerCnt].evt, Sync::Event(true));
		
		Sync::ThreadUtil::Create(WorkerThread, &this->workers[workerCnt]);
	}
}

Net::TCPClientMgr::~TCPClientMgr()
{
	UOSInt i = this->cliMap.GetCount();
	Net::TCPClientMgr::TCPClientStatus *cliStat;
	if (i)
	{
		while (i-- > 0)
		{
			Sync::MutexUsage mutUsage(this->cliMut);
			cliStat = this->cliMap.GetItem(i);
			mutUsage.EndUse();
			if (cliStat)
			{
				cliStat->cli->Close();
			}
		}
	}
	while (this->cliMap.GetCount() > 0)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->toStop = true;
	((Sync::Event*)this->clsData)->Set();
	while (clientThreadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	i = this->workerCnt;
	while (i-- > 0)
	{
		this->workers[i].toStop = true;
		this->workers[i].evt->Set();
	}
	Bool exist = true;
	while (exist)
	{
		exist = false;
		i = this->workerCnt;
		while (i-- > 0)
		{
			if (this->workers[i].state != WorkerState::Stopped)
			{
				exist = true;
				break;
			}
		}
		if (!exist)
			break;
		Sync::SimpleThread::Sleep(10);
	}
	i = this->workerCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->workers[i].evt);
	}
	MemFree(this->workers);

	Sync::Event *recvEvt = ((Sync::Event*)this->clsData);
	DEL_CLASS(recvEvt);
	SDEL_CLASS(this->logWriter);
}

void Net::TCPClientMgr::SetLogFile(Text::CStringNN logFile)
{
	SDEL_CLASS(this->logWriter);
	NEW_CLASS(this->logWriter, IO::SMTCWriter(logFile));
}

void Net::TCPClientMgr::AddClient(NotNullPtr<TCPClient> cli, void *cliData)
{
	cli->SetNoDelay(true);
	cli->SetTimeout(this->timeout);
	Sync::MutexUsage mutUsage(this->cliMut);
	Net::TCPClientMgr::TCPClientStatus *cliStat;
	if (this->logWriter) this->logWriter->TCPConnect(cli);
	NEW_CLASS(cliStat, Net::TCPClientMgr::TCPClientStatus());
	cliStat->cli = cli;
	cliStat->cliData = cliData;
	cliStat->lastDataTime = Data::Timestamp::UtcNow();
	cliStat->reading = false;
	cliStat->processing = false;
	cliStat->timeAlerted = false;
	cliStat->timeStart = 0;
	Sync::MutexUsage readMutUsage(cliStat->readMut);
	cliStat->reading = true;
	this->cliMap.Put(cli->GetCliId(), cliStat);
	cliStat->readReq = cliStat->cli->BeginRead(Data::ByteArray(cliStat->buff, TCP_BUFF_SIZE), ((Sync::Event*)this->clsData));
}

Bool Net::TCPClientMgr::SendClientData(UInt64 cliId, const UInt8 *data, UOSInt buffSize)
{
	Net::TCPClientMgr::TCPClientStatus *cliStat;
	{
		Sync::MutexUsage mutUsage(this->cliMut);
		cliStat = this->cliMap.Get(cliId);
	}
	if (cliStat)
	{
		if (this->logWriter) this->logWriter->TCPSend(cliStat->cli, data, buffSize);
		return cliStat->cli->Write(data, buffSize) == buffSize;
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
		Net::TCPClientMgr::TCPClientStatus *cliStat;
		cliStat = (Net::TCPClientMgr::TCPClientStatus*)this->cliMap.GetItem(i);
		if (cliStat)
		{
			cliStat->cli->Close();
		}
	}
	mutUsage.EndUse();
}

void Net::TCPClientMgr::UseGetClient(NotNullPtr<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->cliMut);
}

UOSInt Net::TCPClientMgr::GetClientCount() const
{
	return this->cliMap.GetCount();
}

void Net::TCPClientMgr::ExtendTimeout(NotNullPtr<Net::TCPClient> cli)
{
	Sync::MutexUsage mutUsage(this->cliMut);
	Net::TCPClientMgr::TCPClientStatus *cliStat = this->cliMap.Get(cli->GetCliId());
	if (cliStat)
	{
		cliStat->lastDataTime = Data::Timestamp::UtcNow();
	}
}

Net::TCPClient *Net::TCPClientMgr::GetClient(UOSInt index, void **cliData)
{
	Net::TCPClientMgr::TCPClientStatus *cliStat = this->cliMap.GetItem(index);
	if (cliStat)
	{
		*cliData = cliStat->cliData;
		return cliStat->cli.Ptr();
	}
	return 0;
}

IO::SMTCWriter *Net::TCPClientMgr::GetLogWriter() const
{
	return this->logWriter;
}