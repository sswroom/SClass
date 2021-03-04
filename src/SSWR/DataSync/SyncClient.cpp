#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoSyncHandler.h"
#include "Net/TCPClient.h"
#include "SSWR/DataSync/SyncClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall SSWR::DataSync::SyncClient::RecvThread(void *userObj)
{
	SSWR::DataSync::SyncClient *me = (SSWR::DataSync::SyncClient*)userObj;
	OSInt recvSize;
	UInt8 *buff;
	OSInt buffSize;
	me->recvRunning = true;
	buff = MemAlloc(UInt8, 8192);
	buffSize = 0;
	while (!me->toStop)
	{
		if (me->cli)
		{
			recvSize = me->cli->Read(&buff[buffSize], 8192 - buffSize);
			if (recvSize <= 0)
			{
				Sync::MutexUsage mutUsage(me->cliMut);
				DEL_CLASS(me->cli);
				me->cli = 0;
				mutUsage.EndUse();
			}
			else
			{
				buffSize += recvSize;
				recvSize = me->protoHdlr->ParseProtocol(me->cli, 0, 0, buff, buffSize);
				if (recvSize <= 0)
				{
					buffSize = 0;
				}
				else if (recvSize < buffSize)
				{
					MemCopyO(buff, &buff[buffSize - recvSize], recvSize);
					buffSize = recvSize;
				}
			}
		}
		else
		{

			Net::TCPClient *cli;
			NEW_CLASS(cli, Net::TCPClient(me->sockf, me->cliHost, me->cliPort));
			if (cli->IsClosed())
			{
				DEL_CLASS(cli);
				me->recvEvt->Wait(1000);
			}
			else
			{
				Sync::MutexUsage mutUsage(me->cliMut);
				me->cli = cli;
				me->cliKATime->SetCurrTimeUTC();
				mutUsage.EndUse();
				buffSize = 0;
				me->SendLogin();
			}
		}
	}
	if (me->cli)
	{
		Sync::MutexUsage mutUsage(me->cliMut);
		DEL_CLASS(me->cli);
		me->cli = 0;
		mutUsage.EndUse();
	}
	MemFree(buff);
	me->recvRunning = false;
	return 0;
}

UInt32 __stdcall SSWR::DataSync::SyncClient::KAThread(void *userObj)
{
	SSWR::DataSync::SyncClient *me = (SSWR::DataSync::SyncClient*)userObj;
	Data::DateTime *currTime;
	UOSInt i;
	UOSInt j;
	me->kaRunning = true;
	NEW_CLASS(currTime, Data::DateTime());
	while (!me->toStop)
	{
		if (me->cli)
		{
			currTime->SetCurrTimeUTC();
			Sync::MutexUsage mutUsage(me->cliMut);
			if (me->cli && currTime->DiffMS(me->cliKATime) >= 120000)
			{
				me->cliKATime->SetCurrTimeUTC();
				mutUsage.EndUse();
				me->SendKA();
			}
			else
			{
				mutUsage.EndUse();
			}
		}

		i = 0;
		j = me->dataMgr->GetCount();
		while (i < j)
		{
			UOSInt dataSize;
			const UInt8 *buff = me->dataMgr->GetData(i, &dataSize);
			if (!me->SendUserData(buff, dataSize))
			{
				break;
			}
			i++;
		}
		if (i > 0)
		{
			me->dataMgr->RemoveData(i);
		}
		else if (j > 16384)
		{
			me->dataMgr->RemoveData(j - 16384);
		}
		me->kaEvt->Wait(1000);
	}
	DEL_CLASS(currTime);
	me->kaRunning = false;
	return 0;
}

Bool SSWR::DataSync::SyncClient::SendLogin()
{
	Bool succ = false;
	UInt8 cmdBuff[512];
	UInt8 packetBuff[512];
	UOSInt len;
	WriteInt32(cmdBuff, this->serverId);
	len = Text::StrCharCnt(this->serverName);
	cmdBuff[4] = (UInt8)(len & 0xff);
	Text::StrConcat((UTF8Char*)&cmdBuff[5], this->serverName);
	len = this->protoHdlr->BuildPacket(packetBuff, 0, 0, cmdBuff, len + 5, 0);
	
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		succ = (this->cli->Write(packetBuff, len) == len);
	}
	mutUsage.EndUse();
	return succ;
}

Bool SSWR::DataSync::SyncClient::SendKA()
{
	Bool succ = false;
	UInt8 packetBuff[32];
	UOSInt len;
	len = this->protoHdlr->BuildPacket(packetBuff, 2, 0, 0, 0, 0);
	
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		succ = (this->cli->Write(packetBuff, len) == len);
	}
	mutUsage.EndUse();
	return succ;
}

Bool SSWR::DataSync::SyncClient::SendUserData(const UInt8 *data, UOSInt dataSize)
{
	Bool succ = false;
	UInt8 packetBuff[2048];
	UOSInt len;
	if (dataSize > 2038)
	{
		UInt8 *dataBuff = MemAlloc(UInt8, dataSize + 10);
		len = this->protoHdlr->BuildPacket(dataBuff, 4, 0, data, dataSize, 0);
		
		Sync::MutexUsage mutUsage(this->cliMut);
		if (this->cli)
		{
			succ = (this->cli->Write(dataBuff, len) == len);
		}
		mutUsage.EndUse();
		MemFree(dataBuff);
	}
	else
	{
		len = this->protoHdlr->BuildPacket(packetBuff, 4, 0, data, dataSize, 0);
		
		Sync::MutexUsage mutUsage(this->cliMut);
		if (this->cli)
		{
			succ = (this->cli->Write(packetBuff, len) == len);
		}
		mutUsage.EndUse();
	}
	return succ;
}

SSWR::DataSync::SyncClient::SyncClient(Net::SocketFactory *sockf, Int32 serverId, const UTF8Char *serverName, const UTF8Char *clientHost, UInt16 cliPort)
{
	this->sockf = sockf;
	NEW_CLASS(this->protoHdlr, IO::ProtoHdlr::ProtoSyncHandler(this));
	this->serverId = serverId;
	this->serverName = Text::StrCopyNew(serverName);
	NEW_CLASS(this->cliMut, Sync::Mutex());
	NEW_CLASS(this->cliKATime, Data::DateTime());
	this->cli = 0;
	this->cliHost = Text::StrCopyNew(clientHost);
	this->cliPort = cliPort;
	NEW_CLASS(this->dataMgr, SSWR::DataSync::SyncClientDataMgr());

	NEW_CLASS(this->recvEvt, Sync::Event(true, (const UTF8Char*)"SSWR.DataSync.SyncClient.recvEvt"));
	NEW_CLASS(this->kaEvt, Sync::Event(true, (const UTF8Char*)"SSWR.DataSync.SyncClient.kaEvt"));
	this->recvRunning = false;
	this->kaRunning = false;
	this->toStop = false;

	Sync::Thread::Create(RecvThread, this);
	Sync::Thread::Create(KAThread, this);
	while (!this->recvRunning || !this->kaRunning)
	{
		Sync::Thread::Sleep(1);
	}
}

SSWR::DataSync::SyncClient::~SyncClient()
{
	this->toStop = true;
	this->recvEvt->Set();
	this->kaEvt->Set();
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Close();
	}
	mutUsage.EndUse();
	while (this->recvRunning || this->kaRunning)
	{
		Sync::Thread::Sleep(1);
	}
	DEL_CLASS(this->dataMgr);
	DEL_CLASS(this->cliKATime);
	DEL_CLASS(this->cliMut);
	Text::StrDelNew(this->cliHost);
	Text::StrDelNew(this->serverName);
	DEL_CLASS(this->protoHdlr);
	DEL_CLASS(this->recvEvt);
	DEL_CLASS(this->kaEvt);
}

void SSWR::DataSync::SyncClient::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
}

void SSWR::DataSync::SyncClient::DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}

void SSWR::DataSync::SyncClient::AddUserData(const UInt8 *data, UOSInt dataSize)
{
	this->dataMgr->AddUserData(data, dataSize);
}
