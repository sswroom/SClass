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
#include <poll.h>
#include <unistd.h>
#include <stdio.h>

/*
TODO:
http://man7.org/linux/man-pages/man2/pipe.2.html
http://man7.org/linux/man-pages/man2/poll.2.html
*/

struct Net::TCPClientMgr::ClassData
{
	int piperdfd;
	int pipewrfd;
	Bool hasData;
};

void TCPClientMgr_RemoveCliStat(Data::ArrayList<Net::TCPClientMgr::TCPClientStatus*> *cliArr, Data::ArrayListUInt64 *cliIdArr, Net::TCPClientMgr::TCPClientStatus *cliStat)
{
	OSInt ind;
	UOSInt i;
	UInt64 cliId = cliStat->cli->GetCliId();
//	printf("Removing CliId: %lld\r\n", cliId); //////////

	ind = cliIdArr->SortedIndexOf(cliId);
	if (ind < 0)
	{
		printf("CliId not found\r\n");
	}
	if (cliArr->GetItem((UOSInt)ind) == cliStat)
	{
		cliArr->RemoveAt((UOSInt)ind);
		cliIdArr->RemoveAt((UOSInt)ind);
	}
	else
	{
		i = cliArr->IndexOf(cliStat);
		if (i != INVALID_INDEX)
		{
			cliArr->RemoveAt(i);
			cliIdArr->RemoveAt(i);
		}
		else
		{
			printf("CliId not found2\r\n");
		}
	}
}

UInt32 __stdcall Net::TCPClientMgr::ClientThread(void *o)
{
	Net::TCPClientMgr *me = (Net::TCPClientMgr*)o;
	ClassData *clsData = me->clsData;
	Data::Timestamp currTime;
	OSInt pollRet;
	UOSInt i;
	UOSInt readSize;
	UInt8 tmpBuff[16];
	Net::TCPClientMgr::TCPClientStatus *cliStat;
	UOSInt pollfdCap = 16;
	struct pollfd *pollfds = MemAlloc(struct pollfd, pollfdCap);
	Net::TCPClientMgr::TCPClientStatus **pollCli = MemAlloc(Net::TCPClientMgr::TCPClientStatus*, pollfdCap);
	UOSInt pollCliCnt;
	UOSInt pollReqCnt;
	Bool pollPreData = false;

	Sync::ThreadUtil::SetName((const UTF8Char*)"TCPCliMgr");
	me->clientThreadRunning = true;
	Sync::ThreadUtil::SetPriority(Sync::ThreadUtil::TP_REALTIME);
	while (!me->toStop)
	{
		pollPreData = false;
		Sync::MutexUsage mutUsage(&me->cliMut);
		pollCliCnt = me->cliArr.GetCount();
		if (pollCliCnt >= pollfdCap)
		{
			while (pollCliCnt >= pollfdCap)
			{
				pollfdCap = pollfdCap << 1;
			}
			MemFree(pollfds);
			MemFree(pollCli);
			pollfds = MemAlloc(struct pollfd, pollfdCap);
			pollCli = MemAlloc(Net::TCPClientMgr::TCPClientStatus*, pollfdCap);
		}
		pollReqCnt = 0;
		pollfds[0].fd = clsData->piperdfd;
		pollfds[0].events = POLLIN;
		pollfds[0].revents = 0;
		if (clsData->hasData)
		{
			pollfds[0].revents = POLLIN;
			pollPreData = true;
		}
		i = 0;
		while (i < pollCliCnt)
		{
			cliStat = me->cliArr.GetItem(i);
			if (cliStat->reading)
			{
				Socket *s = cliStat->cli->GetSocket();
				if (s == 0 || cliStat->cli->IsClosed())
				{
					TCPClientMgr_RemoveCliStat(&me->cliArr, &me->cliIdArr, cliStat);
					Sync::MutexUsage readMutUsage(&cliStat->readMut);
					readMutUsage.EndUse();
					me->evtHdlr(cliStat->cli, me->userObj, cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_DISCONNECT);
					DEL_CLASS(cliStat);
					i--;
					pollCliCnt--;
				}
				else if (cliStat->reading)
				{
					pollCli[pollReqCnt] = cliStat;
					pollReqCnt++;
					pollfds[pollReqCnt].fd = -1 + (int)(OSInt)s;
					pollfds[pollReqCnt].events = POLLIN;
					pollfds[pollReqCnt].revents = 0;
					if (cliStat->recvDataExist)
					{
						pollfds[pollReqCnt].revents = POLLIN;
						pollPreData = true;
					}
				}
			}

			i++;
		}
		mutUsage.EndUse();
		if (pollPreData)
		{
			pollRet = 1;
		}
		else
		{
//			printf("Poll Begin\r\n");
			pollRet = poll(pollfds, pollReqCnt + 1, -1);
//			printf("Poll End %d\r\n", i);
		}
		if (pollRet > 0)
		{
			if (pollfds[0].revents != 0)
			{
				pollRet = read(clsData->piperdfd, tmpBuff, 16);
				pollfds[0].revents = 0;
				clsData->hasData = false;
			}
			i = pollReqCnt;
			while (i-- > 0)
			{
				if (pollfds[i + 1].revents != 0)
				{
//					printf("Cli %d revents %d\r\n", i, pollfds[i + 1].revents);
					pollfds[i + 1].revents = 0;
					Sync::MutexUsage mutUsage(&me->cliMut);
					cliStat = pollCli[i];
					if (cliStat && cliStat->reading)
					{
						Bool closed = false;
						Sync::MutexUsage readMutUsage(&cliStat->readMut);
						if (cliStat->cli->IsClosed())
						{
							Sync::SimpleThread::Sleep(1);
							closed = true;
						}
						readSize = cliStat->cli->GetRecvBuffSize();
						if (!closed && readSize <= 0)
						{
//							printf("Cli Empty data found\r\n");
							cliStat->recvDataExist = false;
							readMutUsage.EndUse();
							readSize = 0;
							cliStat->cli->ShutdownSend();
							cliStat->cli->Close();
						}
						else
						{
//							printf("Cli Read Begin %d\r\n", readSize);
							readSize = cliStat->cli->Read(Data::ByteArray(cliStat->buff, TCP_BUFF_SIZE));
//							printf("Cli Read End %d\r\n", readSize);
							readMutUsage.EndUse();
						}

						if (readSize == (UOSInt)-1)
						{
//							printf("Cli readSize = -1\r\n");
							currTime = Data::Timestamp::UtcNow();
							if (currTime.Diff(cliStat->lastDataTime) > me->timeout)
							{
//								printf("Cli disconnect\r\n");
								cliStat->cli->ShutdownSend();
								cliStat->cli->Close();
								Sync::SimpleThread::Sleep(1);
								TCPClientMgr_RemoveCliStat(&me->cliArr, &me->cliIdArr, cliStat);
								mutUsage.EndUse();

								me->evtHdlr(cliStat->cli, me->userObj, cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_DISCONNECT);
								DEL_CLASS(cliStat);
							}
							else
							{
								mutUsage.EndUse();
							}
						}
						else if (readSize)
						{
//							printf("Cli read data\r\n");
							cliStat->reading = false;
							cliStat->lastDataTime = Data::Timestamp::UtcNow();
							mutUsage.EndUse();
							me->evtHdlr(cliStat->cli, me->userObj, cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_HASDATA);
							cliStat->buffSize = readSize;
							me->ProcessClient(cliStat);
						}
						else
						{
//							printf("Cli end conn\r\n");
							TCPClientMgr_RemoveCliStat(&me->cliArr, &me->cliIdArr, cliStat);
							mutUsage.EndUse();

							me->evtHdlr(cliStat->cli, me->userObj, cliStat->cliData, Net::TCPClientMgr::TCP_EVENT_DISCONNECT);
							DEL_CLASS(cliStat);
						}
					}
					else
					{
						mutUsage.EndUse();
					}
				}
			}
		}
	}
	MemFree(pollfds);
	MemFree(pollCli);
	me->clientThreadRunning = false;
	return 0;
}

UInt32 __stdcall Net::TCPClientMgr::WorkerThread(void *o)
{
	Net::TCPClientMgr::WorkerStatus *stat = (Net::TCPClientMgr::WorkerStatus*)o;
	Net::TCPClientMgr *me = stat->me;
	ClassData *clsData = me->clsData;
	Data::Timestamp currTime;
	Net::TCPClientMgr::TCPClientStatus *cliStat;
	Data::Timestamp lastCheckTime = 0;
	UOSInt i;
	UTF8Char sbuff[16];
	Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("TCPCliMgr")), stat->index);
	Sync::ThreadUtil::SetName(sbuff);
	{
		Sync::Event evt;
		stat->evt = &evt;
		stat->state = WorkerState::Idle;
		while (!stat->toStop)
		{
			while ((cliStat = me->workerTasks.Get()) != 0)
			{
				stat->state = WorkerState::Processing;
				currTime = Data::Timestamp::UtcNow();
				cliStat->timeStart = currTime;
				cliStat->timeAlerted = false;
				cliStat->processing = true;
				me->dataHdlr(cliStat->cli, me->userObj, cliStat->cliData, cliStat->buff, cliStat->buffSize);
				cliStat->processing = false;
				Sync::MutexUsage mutUsage(&cliStat->readMut);
				cliStat->recvDataExist = cliStat->cli->GetRecvBuffSize() > 0;
				cliStat->reading = true;
				mutUsage.EndUse();
				clsData->hasData = true;
				if (write(clsData->pipewrfd, "", 1) == -1)
				{
					printf("TCPClientMgr: Error in writing to pipe\r\n");
				}
			}
			stat->state = WorkerState::Idle;
			if (stat->isPrimary)
			{
				currTime = Data::Timestamp::UtcNow();
				if (currTime.DiffMS(lastCheckTime) >= 10000)
				{
					lastCheckTime = currTime;
					Sync::MutexUsage mutUsage(&me->cliMut);
					i = me->cliArr.GetCount();
					while (i-- > 0)
					{
						cliStat = me->cliArr.GetItem(i);
						if (currTime.Diff(cliStat->lastDataTime) > cliStat->cli->GetTimeout())
						{
			//				printf("Client data timeout\r\n");
							cliStat->cli->Close();
						}
					}
					mutUsage.EndUse();
				}
			}
			stat->evt->Wait(700);
		}
	}
	stat->state = WorkerState::Stopped;
	return 0;
}

void Net::TCPClientMgr::ProcessClient(Net::TCPClientMgr::TCPClientStatus *cliStat)
{
	if (!Text::StrEqualsC(cliStat->debug, 5, UTF8STRC("debug")))
	{
		printf("ProcessClient: not cliStat\r\n");
	}
	this->workerTasks.Put(cliStat);
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

Net::TCPClientMgr::TCPClientMgr(Int32 timeOutSeconds, TCPClientEvent evtHdlr, TCPClientData dataHdlr, void *userObj, UOSInt workerCnt, TCPClientTimeout toHdlr)
{
	this->timeout = Data::Duration(timeOutSeconds, 0);
	this->evtHdlr = evtHdlr;
	this->dataHdlr = dataHdlr;
	this->toHdlr = toHdlr;
	this->userObj = userObj;
	this->toStop = false;
	this->clientThreadRunning = false;
	if (workerCnt <= 0)
		workerCnt = 1;
	this->workerCnt = workerCnt;
	ClassData *clsData = 0;
	int pipefd[2];
	if (pipe(pipefd) != -1)
	{
		clsData = MemAlloc(ClassData, 1);
		clsData->piperdfd = pipefd[0];
		clsData->pipewrfd = pipefd[1];
		clsData->hasData = false;
	}
	this->clsData = clsData;
	if (clsData == 0)
	{
		this->workers = 0;
	}
	else
	{
		Sync::ThreadUtil::Create(ClientThread, this);
		this->workers = MemAlloc(WorkerStatus, workerCnt);
		while (workerCnt-- > 0)
		{
			this->workers[workerCnt].index = workerCnt;
			this->workers[workerCnt].state = WorkerState::NotStarted;
			this->workers[workerCnt].toStop = false;
			this->workers[workerCnt].isPrimary = (workerCnt == 0);
			this->workers[workerCnt].me = this;

			Sync::ThreadUtil::Create(WorkerThread, &this->workers[workerCnt]);
		}
	}
}

Net::TCPClientMgr::~TCPClientMgr()
{
	ClassData *clsData = this->clsData;
	UOSInt i = this->cliArr.GetCount();
	Net::TCPClientMgr::TCPClientStatus *cliStat;
	if (i)
	{
		while (i-- > 0)
		{
			Sync::MutexUsage mutUsage(&this->cliMut);
			cliStat = this->cliArr.GetItem(i);
			mutUsage.EndUse();
			if (cliStat)
			{
				cliStat->cli->Close();
			}
		}
	}
	if (clsData)
	{
		clsData->hasData = true;
		i = (UOSInt)write(clsData->pipewrfd, "", 1);
	}
	while (this->cliArr.GetCount() > 0)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->toStop = true;
	if (clsData)
	{
		clsData->hasData = true;
		i = (UOSInt)write(clsData->pipewrfd, "", 1);
	}
	while (clientThreadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	if (this->workers)
	{
		i = this->workerCnt;
		while (i-- > 0)
		{
			this->workers[i].toStop = true;
			if (this->workers[i].state != WorkerState::NotStarted)
			{
				this->workers[i].evt->Set();
			}
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
		MemFree(this->workers);
	}

	if (clsData)
	{
		close(clsData->pipewrfd);
		close(clsData->piperdfd);
		MemFree(clsData);
	}
}

void Net::TCPClientMgr::AddClient(NotNullPtr<TCPClient> cli, void *cliData)
{
//	printf("Client added and set timeout to %d\r\n", this->timeOutSeconds);
	cli->SetNoDelay(true);
	cli->SetTimeout(this->timeout);
	UInt64 cliId = cli->GetCliId();
	if (cliId == 0)
	{
		this->evtHdlr(cli, this->userObj, cliData, Net::TCPClientMgr::TCP_EVENT_DISCONNECT);
		return;
	}
//	printf("Adding Client Id %lld\r\n", cliId);
	Sync::MutexUsage mutUsage(&this->cliMut);
	Net::TCPClientMgr::TCPClientStatus *cliStat;
	NEW_CLASS(cliStat, Net::TCPClientMgr::TCPClientStatus());
	Text::StrConcatC(cliStat->debug, UTF8STRC("debug"));
	cliStat->cli = cli;
	cliStat->cliData = cliData;
	cliStat->lastDataTime = Data::Timestamp::UtcNow();
	cliStat->reading = false;
	cliStat->processing = false;
	cliStat->timeAlerted = false;
	cliStat->timeStart = 0;
	Sync::MutexUsage readMutUsage(&cliStat->readMut);
	cliStat->reading = true;
	cliStat->recvDataExist = false;
	OSInt i = this->cliIdArr.SortedIndexOf(cliId);
	if (i >= 0)
	{
		printf("TCPClientMgr: Duplicate Client Id %llx\r\n", cliId);
	}
	else
	{
		this->cliIdArr.Insert((UOSInt)~i, cliId);
		this->cliArr.Insert((UOSInt)~i, cliStat);
	}
	readMutUsage.EndUse();
	mutUsage.EndUse();
	if (write((this->clsData)->pipewrfd, "", 1) == -1)
	{
		printf("TCPClientMgr: Error in writing to pipe\r\n");
	}
}

Bool Net::TCPClientMgr::SendClientData(UInt64 cliId, const UInt8 *buff, UOSInt buffSize)
{
	OSInt i;
	Net::TCPClientMgr::TCPClientStatus *cliStat = 0;
	Sync::MutexUsage mutUsage(&this->cliMut);
	i = this->cliIdArr.SortedIndexOf(cliId);
	if (i >= 0)
	{
		cliStat = this->cliArr.GetItem((UOSInt)i);
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
	UOSInt i = this->cliArr.GetCount();
	Sync::MutexUsage mutUsage(&this->cliMut);
	while (i-- > 0)
	{
		Net::TCPClientMgr::TCPClientStatus *cliStat;
		cliStat = (Net::TCPClientMgr::TCPClientStatus*)this->cliArr.GetItem(i);
		if (cliStat)
		{
			cliStat->cli->Close();
		}
	}
	mutUsage.EndUse();
}

void Net::TCPClientMgr::UseGetClient(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(&this->cliMut);
}

UOSInt Net::TCPClientMgr::GetClientCount() const
{
	return this->cliArr.GetCount();
}

void Net::TCPClientMgr::ExtendTimeout(NotNullPtr<Net::TCPClient> cli)
{
	Sync::MutexUsage mutUsage(&this->cliMut);
	OSInt i = this->cliIdArr.SortedIndexOf(cli->GetCliId());
	if (i >= 0)
	{
		Net::TCPClientMgr::TCPClientStatus *cliStat = this->cliArr.GetItem((UOSInt)i);
		cliStat->lastDataTime = Data::Timestamp::UtcNow();
	}
}

Net::TCPClient *Net::TCPClientMgr::GetClient(UOSInt index, void **cliData)
{
	Net::TCPClientMgr::TCPClientStatus *cliStat = this->cliArr.GetItem(index);
	if (cliStat)
	{
		*cliData = cliStat->cliData;
		return cliStat->cli.Ptr();
	}
	return 0;
}
