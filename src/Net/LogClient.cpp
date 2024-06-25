#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "Net/LogClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

#define MAXSIZE 4096
#define MTU 2048
#define BUFFSIZE (MAXSIZE + MTU)

UInt32 __stdcall Net::LogClient::RecvThread(AnyType userObj)
{
	NN<Net::LogClient> me = userObj.GetNN<Net::LogClient>();
	UOSInt recvSize = 0;
	UOSInt readSize;
	NN<Net::TCPClient> cli;
	me->recvRunning = true;
	{
		Data::ByteBuffer recvBuff(BUFFSIZE);
		while (!me->recvToStop)
		{
			if (cli.Set(me->cli))
			{
				readSize = cli->Read(recvBuff.SubArray(recvSize));
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
					readSize = me->protoHdlr.ParseProtocol(cli, me,0, recvBuff.WithSize(recvSize));
					if (readSize <= 0)
					{
						recvSize = 0;
					}
					else if (readSize < recvSize)
					{
						recvBuff.CopyInner(0, recvSize - readSize, readSize);
						recvSize = readSize;
					}
				}
			}
			else
			{
				me->recvEvt.Wait(1000);
			}
		}

		if (me->cli)
		{
			Sync::MutexUsage mutUsage(me->cliMut);
			DEL_CLASS(me->cli);
			me->cli = 0;
			mutUsage.EndUse();
		}
	}
	me->recvRunning = false;
	return 0;
}

UInt32 __stdcall Net::LogClient::SendThread(AnyType userObj)
{
	NN<Net::LogClient> me = userObj.GetNN<Net::LogClient>();
	Net::TCPClient *cli;
	Int64 t;
	Int64 msgTime;
	NN<Text::String> msg;
	UOSInt msgLen;
	Int64 nextKATime = 0;
	UInt8 kaBuff[10];
	UOSInt buffSize;

	me->sendRunning = true;
	while (!me->sendToStop)
	{
		if (me->cli == 0)
		{
			NEW_CLASS(cli, Net::TCPClient(me->sockf, me->addr, me->port, me->timeout));
			if (cli->IsConnectError())
			{
				DEL_CLASS(cli);
			}
			else
			{
				Sync::MutexUsage mutUsage(me->cliMut);
				me->cli = cli;
				mutUsage.EndUse();
				nextKATime = Data::DateTimeUtil::GetCurrTimeMillis();
			}
		}

		t = Data::DateTimeUtil::GetCurrTimeMillis();
		Sync::MutexUsage mutUsage(me->cliMut);
		if (me->cli)
		{
			if (t >= nextKATime)
			{
				buffSize = me->protoHdlr.BuildPacket(kaBuff, 0, 0, kaBuff, 0, 0);
				me->cli->Write(Data::ByteArrayR(kaBuff, buffSize));
				nextKATime = t + 60000;
			}

			if (t >= me->lastSendTime + 30000)
			{
				Sync::MutexUsage mutUsage(me->mut);
				if (me->msgList.GetItem(0).SetTo(msg))
				{
					UInt8 *buff1;
					UInt8 *buff2;
					msgTime = me->dateList.GetItem(0).ToTicks();
					msgLen = msg->leng;
					buff1 = MemAlloc(UInt8, 8 + msgLen);
					buff2 = MemAlloc(UInt8, 18 + msgLen);
					WriteInt64(buff1, msgTime);
					MemCopyNO(&buff1[8], msg->v.Ptr(), msgLen);
					buffSize = me->protoHdlr.BuildPacket(buff2, 2, 0, buff1, msgLen + 8, 0);
					me->cli->Write(Data::ByteArrayR(buff2, buffSize));
					MemFree(buff1);
					MemFree(buff2);
					
					me->lastSendTime = t;
				}
				mutUsage.EndUse();
			}
		}
		mutUsage.EndUse();
		
		me->sendEvt.Wait(1000);
	}
	me->sendRunning = false;
	return 0;
}

Net::LogClient::LogClient(NN<Net::SocketFactory> sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, Data::Duration timeout) : protoHdlr(*this)
{
	this->sockf = sockf;
	this->addr = *addr;
	this->port = port;
	this->cli = 0;
	this->timeout = timeout;

	this->lastSendTime = 0;
	this->sendRunning = false;
	this->sendToStop = false;
	this->recvRunning = false;
	this->recvToStop = false;
	Sync::ThreadUtil::Create(SendThread, this);
	Sync::ThreadUtil::Create(RecvThread, this);
	while (!this->sendRunning || !this->recvRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Net::LogClient::~LogClient()
{
	this->recvToStop = true;
	this->sendToStop = true;
	this->recvEvt.Set();
	this->sendEvt.Set();
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Close();
	}
	mutUsage.EndUse();
	while (this->sendRunning || this->recvRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->msgList.FreeAll();
}

void Net::LogClient::LogClosed()
{
	DEL_CLASS(this);
}

void Net::LogClient::LogAdded(const Data::Timestamp &time, Text::CStringNN logMsg, LogLevel logLev)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->msgList.Add(Text::String::New(logMsg));
	this->dateList.Add(time);
	mutUsage.EndUse();
	this->sendEvt.Set();
}

void Net::LogClient::DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize)
{
	switch (cmdType)
	{
	case 1: //KA Reply
		break;
	case 3: //Log Reply
		{
			Int64 msgTime = ReadInt64(&cmd[0]);
			Sync::MutexUsage mutUsage(this->mut);
			if (msgTime == this->dateList.GetItem(0).ToTicks())
			{
				this->dateList.RemoveAt(0);
				OPTSTR_DEL(this->msgList.RemoveAt(0));
				this->lastSendTime = 0;
				this->sendEvt.Set();
			}
			mutUsage.EndUse();
		}
		break;
	}
}

void Net::LogClient::DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
}
