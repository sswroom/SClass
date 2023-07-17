#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/TCPClient.h"
#include "SSWR/DataSync/SyncClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

#include "Crypto/Hash/CRC32RIEEE.h"

UInt32 __stdcall SSWR::DataSync::SyncClient::RecvThread(void *userObj)
{
	SSWR::DataSync::SyncClient *me = (SSWR::DataSync::SyncClient*)userObj;
	UOSInt recvSize;
	UInt8 *buff;
	UOSInt buffSize;
	NotNullPtr<Net::TCPClient> cli;
	me->recvRunning = true;
	buff = MemAlloc(UInt8, 8192);
	buffSize = 0;
	while (!me->toStop)
	{
		if (cli.Set(me->cli))
		{
			recvSize = cli->Read(Data::ByteArray(&buff[buffSize], 8192 - buffSize));
			if (recvSize <= 0)
			{
				Sync::MutexUsage mutUsage(&me->cliMut);
				DEL_CLASS(me->cli);
				me->cli = 0;
				mutUsage.EndUse();
			}
			else
			{
				buffSize += recvSize;
				recvSize = me->protoHdlr.ParseProtocol(cli, 0, 0, buff, buffSize);
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
			NEW_CLASSNN(cli, Net::TCPClient(me->sockf, me->cliHost->ToCString(), me->cliPort, me->timeout));
			if (cli->IsClosed())
			{
				cli.Delete();
				me->recvEvt.Wait(1000);
			}
			else
			{
				Sync::MutexUsage mutUsage(&me->cliMut);
				me->cli = cli.Ptr();
				me->cliKATime.SetCurrTimeUTC();
				mutUsage.EndUse();
				buffSize = 0;
				me->SendLogin();
			}
		}
	}
	if (me->cli)
	{
		Sync::MutexUsage mutUsage(&me->cliMut);
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
	UOSInt i;
	UOSInt j;
	me->kaRunning = true;
	{
		Data::DateTime currTime;
		while (!me->toStop)
		{
			if (me->cli)
			{
				currTime.SetCurrTimeUTC();
				Sync::MutexUsage mutUsage(&me->cliMut);
				if (me->cli && currTime.DiffMS(&me->cliKATime) >= 120000)
				{
					me->cliKATime.SetCurrTimeUTC();
					mutUsage.EndUse();
					me->SendKA();
				}
				else
				{
					mutUsage.EndUse();
				}
			}

			i = 0;
			j = me->dataMgr.GetCount();
			while (i < j)
			{
				UOSInt dataSize;
				const UInt8 *buff = me->dataMgr.GetData(i, &dataSize);
				if (!me->SendUserData(buff, dataSize))
				{
					break;
				}
				i++;
			}
			if (i > 0)
			{
				me->dataMgr.RemoveData(i);
			}
			else if (j > 16384)
			{
				me->dataMgr.RemoveData(j - 16384);
			}
			me->kaEvt.Wait(1000);
		}
	}
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
	len = this->serverName->leng;
	cmdBuff[4] = (UInt8)(len & 0xff);
	this->serverName->ConcatTo((UTF8Char*)&cmdBuff[5]);
	len = this->protoHdlr.BuildPacket(packetBuff, 0, 0, cmdBuff, len + 5, 0);
	
	Sync::MutexUsage mutUsage(&this->cliMut);
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
	len = this->protoHdlr.BuildPacket(packetBuff, 2, 0, 0, 0, 0);
	
	Sync::MutexUsage mutUsage(&this->cliMut);
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
		len = this->protoHdlr.BuildPacket(dataBuff, 4, 0, data, dataSize, 0);
		
		Sync::MutexUsage mutUsage(&this->cliMut);
		if (this->cli)
		{
			succ = (this->cli->Write(dataBuff, len) == len);
		}
		mutUsage.EndUse();
		MemFree(dataBuff);
	}
	else
	{
		len = this->protoHdlr.BuildPacket(packetBuff, 4, 0, data, dataSize, 0);
		
		Sync::MutexUsage mutUsage(&this->cliMut);
		if (this->cli)
		{
			succ = (this->cli->Write(packetBuff, len) == len);
		}
		mutUsage.EndUse();
	}
	return succ;
}

SSWR::DataSync::SyncClient::SyncClient(Net::SocketFactory *sockf, Int32 serverId, Text::CString serverName, Text::CString clientHost, UInt16 cliPort, Data::Duration timeout) : protoHdlr(this)
{
	this->sockf = sockf;
	this->serverId = serverId;
	this->serverName = Text::String::New(serverName);
	this->cli = 0;
	this->cliHost = Text::String::New(clientHost);
	this->cliPort = cliPort;
	this->timeout = timeout;

	this->recvRunning = false;
	this->kaRunning = false;
	this->toStop = false;

	Sync::ThreadUtil::Create(RecvThread, this);
	Sync::ThreadUtil::Create(KAThread, this);
	while (!this->recvRunning || !this->kaRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
}

SSWR::DataSync::SyncClient::~SyncClient()
{
	this->toStop = true;
	this->recvEvt.Set();
	this->kaEvt.Set();
	Sync::MutexUsage mutUsage(&this->cliMut);
	if (this->cli)
	{
		this->cli->Close();
	}
	mutUsage.EndUse();
	while (this->recvRunning || this->kaRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
	this->cliHost->Release();
	this->serverName->Release();
}

void SSWR::DataSync::SyncClient::DataParsed(NotNullPtr<IO::Stream> stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
}

void SSWR::DataSync::SyncClient::DataSkipped(NotNullPtr<IO::Stream> stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}

void SSWR::DataSync::SyncClient::AddUserData(const UInt8 *data, UOSInt dataSize)
{
	Crypto::Hash::CRC32RIEEE crc;
	crc.CalcDirect(data, dataSize);
	
	this->dataMgr.AddUserData(data, dataSize);
}
