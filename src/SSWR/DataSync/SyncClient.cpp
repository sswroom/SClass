#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoSyncHandler.h"
#include "Net/TCPClient.h"
#include "SSWR/DataSync/SyncClient.h"
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
				me->cliMut->Lock();
				DEL_CLASS(me->cli);
				me->cli = 0;
				me->cliMut->Unlock();
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
				me->cliMut->Lock();
				me->cli = cli;
				me->cliKATime->SetCurrTimeUTC();
				me->cliMut->Unlock();
				buffSize = 0;
				me->SendLogin();
			}
		}
	}
	if (me->cli)
	{
		me->cliMut->Lock();
		DEL_CLASS(me->cli);
		me->cli = 0;
		me->cliMut->Unlock();
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
	UInt8 *userData;
	me->kaRunning = true;
	NEW_CLASS(currTime, Data::DateTime());
	while (!me->toStop)
	{
		if (me->cli)
		{
			currTime->SetCurrTimeUTC();
			me->cliMut->Lock();
			if (me->cli && currTime->DiffMS(me->cliKATime) >= 120000)
			{
				me->cliKATime->SetCurrTimeUTC();
				me->cliMut->Unlock();
				me->SendKA();
			}
			else
			{
				me->cliMut->Unlock();
			}
		}

		i = 0;
		j = me->userDataList->GetCount();
		while (i < j)
		{
			me->userDataMut->Lock();
			userData = me->userDataList->GetItem(i);
			me->SendUserData(&userData[4], ReadInt32(userData));
			me->userDataMut->Unlock();
			i++;
		}
		if (i > 0)
		{
			Data::ArrayList<UInt8*> tmp;
			me->userDataMut->Lock();
			tmp.AddRange(me->userDataList->GetArray(&j), i);
			me->userDataList->RemoveRange(0, i);
			me->userDataMut->Unlock();
			while (i-- > 0)
			{
				MemFree(tmp.GetItem(i));
			}
		}
		else if (j > 16384)
		{
			Data::ArrayList<UInt8*> tmp;
			i = j - 16384;
			me->userDataMut->Lock();
			tmp.AddRange(me->userDataList->GetArray(&j), i);
			me->userDataList->RemoveRange(0, i);
			me->userDataMut->Unlock();
			while (i-- > 0)
			{
				MemFree(tmp.GetItem(i));
			}
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
	
	this->cliMut->Lock();
	if (this->cli)
	{
		succ = (this->cli->Write(packetBuff, len) == len);
	}
	this->cliMut->Unlock();
	return succ;
}

Bool SSWR::DataSync::SyncClient::SendKA()
{
	Bool succ = false;
	UInt8 packetBuff[32];
	UOSInt len;
	len = this->protoHdlr->BuildPacket(packetBuff, 2, 0, 0, 0, 0);
	
	this->cliMut->Lock();
	if (this->cli)
	{
		succ = (this->cli->Write(packetBuff, len) == len);
	}
	this->cliMut->Unlock();
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
		
		this->cliMut->Lock();
		if (this->cli)
		{
			succ = (this->cli->Write(dataBuff, len) == len);
		}
		this->cliMut->Unlock();
		MemFree(dataBuff);
	}
	else
	{
		len = this->protoHdlr->BuildPacket(packetBuff, 4, 0, data, dataSize, 0);
		
		this->cliMut->Lock();
		if (this->cli)
		{
			succ = (this->cli->Write(packetBuff, len) == len);
		}
		this->cliMut->Unlock();
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
	NEW_CLASS(this->userDataList, Data::ArrayList<UInt8*>());
	NEW_CLASS(this->userDataMut, Sync::Mutex());

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
	OSInt i;
	this->toStop = true;
	this->recvEvt->Set();
	this->kaEvt->Set();
	this->cliMut->Lock();
	if (this->cli)
	{
		this->cli->Close();
	}
	this->cliMut->Unlock();
	while (this->recvRunning || this->kaRunning)
	{
		Sync::Thread::Sleep(1);
	}

	i = this->userDataList->GetCount();
	while (i-- > 0)
	{
		MemFree(this->userDataList->GetItem(i));
	}
	DEL_CLASS(this->userDataList);
	DEL_CLASS(this->userDataMut);
	DEL_CLASS(this->cliKATime);
	DEL_CLASS(this->cliMut);
	Text::StrDelNew(this->cliHost);
	Text::StrDelNew(this->serverName);
	DEL_CLASS(this->protoHdlr);
}

void SSWR::DataSync::SyncClient::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
}

void SSWR::DataSync::SyncClient::DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}

void SSWR::DataSync::SyncClient::AddUserData(const UInt8 *data, UOSInt dataSize)
{
	UInt8 *newData = MemAlloc(UInt8, dataSize + 4);
	WriteInt32(newData, (Int32)dataSize);
	MemCopyNO(&newData[4], data, dataSize);
	this->userDataMut->Lock();
	this->userDataList->Add(newData);
	this->userDataMut->Unlock();
}
