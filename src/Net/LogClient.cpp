#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "Net/LogClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

#define MAXSIZE 4096
#define MTU 2048
#define BUFFSIZE (MAXSIZE + MTU)

UInt32 __stdcall Net::LogClient::RecvThread(void *userObj)
{
	Net::LogClient *me = (Net::LogClient*)userObj;
	UInt8 *recvBuff;
	OSInt recvSize = 0;
	OSInt readSize;
	me->recvRunning = true;
	recvBuff = MemAlloc(UInt8, BUFFSIZE);
	while (!me->recvToStop)
	{
		if (me->cli)
		{
			readSize = me->cli->Read(recvBuff, BUFFSIZE - recvSize);
			if (readSize == 0)
			{
				recvSize = 0;
				Sync::MutexUsage mutUsage(me->cliMut);
				DEL_CLASS(me->cli);
				me->cli = 0;
				mutUsage.EndUse();
			}
			else
			{
				recvSize += readSize;
				readSize = me->protoHdlr->ParseProtocol(me->cli, me,0, recvBuff, recvSize);
				if (readSize <= 0)
				{
					recvSize = 0;
				}
				else if (readSize < recvSize)
				{
					MemCopyO(recvBuff, &recvBuff[recvSize - readSize], readSize);
					recvSize = readSize;
				}
			}
		}
		else
		{
			me->recvEvt->Wait(1000);
		}
	}

	if (me->cli)
	{
		Sync::MutexUsage mutUsage(me->cliMut);
		DEL_CLASS(me->cli);
		me->cli = 0;
		mutUsage.EndUse();
	}
	me->recvRunning = false;
	return 0;
}

UInt32 __stdcall Net::LogClient::SendThread(void *userObj)
{
	Net::LogClient *me = (Net::LogClient*)userObj;
	Net::TCPClient *cli;
	Data::DateTime *currTime;
	Int64 t;
	Int64 msgTime;
	const UTF8Char *msg;
	OSInt msgLen;
	Int64 nextKATime = 0;
	UInt8 kaBuff[10];
	OSInt buffSize;

	me->sendRunning = true;
	NEW_CLASS(currTime, Data::DateTime())
	while (!me->sendToStop)
	{
		if (me->cli == 0)
		{
			NEW_CLASS(cli, Net::TCPClient(me->sockf, &me->addr, me->port));
			if (cli->IsConnectError())
			{
				DEL_CLASS(cli);
			}
			else
			{
				Sync::MutexUsage mutUsage(me->cliMut);
				me->cli = cli;
				mutUsage.EndUse();
				currTime->SetCurrTimeUTC();
				nextKATime = currTime->ToTicks();
			}
		}

		currTime->SetCurrTimeUTC();
		t = currTime->ToTicks();
		Sync::MutexUsage mutUsage(me->cliMut);
		if (me->cli)
		{
			if (t >= nextKATime)
			{
				buffSize = me->protoHdlr->BuildPacket(kaBuff, 0, 0, 0, 0, 0);
				me->cli->Write(kaBuff, buffSize);
				nextKATime = t + 60000;
			}

			if (t >= me->lastSendTime + 30000)
			{
				Sync::MutexUsage mutUsage(me->mut);
				if (me->msgList->GetCount() > 0)
				{
					UInt8 *buff1;
					UInt8 *buff2;
					msgTime = me->dateList->GetItem(0);
					msg = me->msgList->GetItem(0);
					msgLen = Text::StrCharCnt(msg);
					buff1 = MemAlloc(UInt8, 8 + msgLen);
					buff2 = MemAlloc(UInt8, 18 + msgLen);
					WriteInt64(buff1, msgTime);
					MemCopyNO(&buff1[8], msg, msgLen);
					buffSize = me->protoHdlr->BuildPacket(buff2, 2, 0, buff1, msgLen + 8, 0);
					me->cli->Write(buff2, buffSize);
					MemFree(buff1);
					MemFree(buff2);
					
					me->lastSendTime = t;
				}
				mutUsage.EndUse();
			}
		}
		mutUsage.EndUse();
		
		me->sendEvt->Wait(1000);
	}
	DEL_CLASS(currTime);
	me->sendRunning = false;
	return 0;
}

Net::LogClient::LogClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	this->sockf = sockf;
	this->addr = *addr;
	this->port = port;
	this->cli = 0;
	NEW_CLASS(this->cliMut, Sync::Mutex());
	NEW_CLASS(this->protoHdlr, IO::ProtoHdlr::ProtoLogCliHandler(this));

	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->dateList, Data::ArrayListInt64());
	NEW_CLASS(this->msgList, Data::ArrayListStrUTF8());
	this->lastSendTime = 0;
	this->sendRunning = false;
	this->sendToStop = false;
	NEW_CLASS(this->sendEvt, Sync::Event(true, (const UTF8Char*)"Net.LogClient.sendEvt"));
	this->recvRunning = false;
	this->recvToStop = false;
	NEW_CLASS(this->recvEvt, Sync::Event(true, (const UTF8Char*)"Net.LogClient.recvEvt"));
	Sync::Thread::Create(SendThread, this);
	Sync::Thread::Create(RecvThread, this);
	while (!this->sendRunning || !this->recvRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

Net::LogClient::~LogClient()
{
	OSInt i;
	this->recvToStop = true;
	this->sendToStop = true;
	this->recvEvt->Set();
	this->sendEvt->Set();
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Close();
	}
	mutUsage.EndUse();
	while (this->sendRunning || this->recvRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->recvEvt);
	DEL_CLASS(this->sendEvt);
	i = this->msgList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->msgList->GetItem(i));
	}
	DEL_CLASS(this->msgList);
	DEL_CLASS(this->dateList);
	DEL_CLASS(this->mut);
	DEL_CLASS(this->protoHdlr);
	DEL_CLASS(this->cliMut);
}

void Net::LogClient::LogClosed()
{
	DEL_CLASS(this);
}

void Net::LogClient::LogAdded(Data::DateTime *time, const UTF8Char *logMsg, LogLevel logLev)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->msgList->Add(Text::StrCopyNew(logMsg));
	this->dateList->Add(time->ToTicks());
	mutUsage.EndUse();
	this->sendEvt->Set();
}

void Net::LogClient::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	switch (cmdType)
	{
	case 1: //KA Reply
		break;
	case 3: //Log Reply
		{
			Int64 msgTime = ReadInt64(cmd);
			Sync::MutexUsage mutUsage(this->mut);
			if (msgTime == this->dateList->GetItem(0))
			{
				this->dateList->RemoveAt(0);
				Text::StrDelNew(this->msgList->RemoveAt(0));
				this->lastSendTime = 0;
				this->sendEvt->Set();
			}
			mutUsage.EndUse();
		}
		break;
	}
}

void Net::LogClient::DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}
