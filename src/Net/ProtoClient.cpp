#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ProtoClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

UInt32 __stdcall Net::ProtoClient::ProtoThread(void *userObj)
{
	Net::ProtoClient *me = (Net::ProtoClient *)userObj;
	UInt8 buff[4096];
	OSInt buffSize;
	OSInt readSize;


	me->threadRunning = true;
	while (!me->threadToStop)
	{
		if (me->started && me->cli == 0)
		{
			Sync::MutexUsage mutUsage(me->cliMut);
			NEW_CLASS(me->cli, Net::TCPClient(me->sockf, &me->cliAddr, me->cliPort));
			if (me->cli->IsConnectError())
			{
				DEL_CLASS(me->cli);
				me->cli = 0;
			}
			else
			{
				me->cliData = me->proto->CreateStreamData(me->cli);
				buffSize = 0;
				me->connected = true;
				mutUsage.EndUse();
				me->cliHdlr->ClientConn();
			}
		}
		if (me->cli)
		{
			readSize = me->cli->Read(&buff[buffSize], 2048);
			if (readSize == 0)
			{
				Sync::MutexUsage mutUsage(me->cliMut);
				me->proto->DeleteStreamData(me->cli, me->cliData);
				DEL_CLASS(me->cli);
				me->cli = 0;
				me->connected = false;
				mutUsage.EndUse();
				me->cliHdlr->ClientDisconn();
			}
			else
			{
				buffSize += readSize;
				readSize = me->proto->ParseProtocol(me->cli, 0, me->cliData, buff, buffSize);
				if (readSize == 0)
				{
					buffSize = 0;
				}
				else if (buffSize != readSize)
				{
					MemCopyO(buff, &buff[buffSize - readSize], readSize);
					buffSize = readSize;
				}
				if (buffSize > 2048)
				{
					MemCopyO(buff, &buff[buffSize - 2048], 2048);
					buffSize = 2048;
				}
			}
		}

		if (me->cli == 0)
		{
			me->threadEvt.Wait(10000);
		}
	}
	if (me->cli)
	{
		Sync::MutexUsage mutUsage(me->cliMut);
		me->proto->DeleteStreamData(me->cli, me->cliData);
		DEL_CLASS(me->cli);
		me->cli = 0;
		me->connected = false;
		mutUsage.EndUse();
		me->cliHdlr->ClientDisconn();
	}
	me->threadRunning = false;
	return 0;
}

Net::ProtoClient::ProtoClient(NN<Net::SocketFactory> sockf, Text::CString cliAddr, UInt16 cliPort, IO::ProtocolHandler *proto, Net::ProtoClient::IProtoClientHandler *cliHdlr)
{
	this->sockf = sockf;
	this->cli = 0;
	Net::SocketUtil::GetIPAddr(cliAddr, &this->cliAddr);
	this->cliPort = cliPort;
	this->proto = proto;
	this->cliHdlr = cliHdlr;
	this->started = false;
	this->connected = false;
	this->threadRunning = false;
	this->threadToStop = false;
	Sync::ThreadUtil::Create(ProtoThread, this);
	while (!this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Net::ProtoClient::~ProtoClient()
{
	this->threadToStop = true;
	{
		Sync::MutexUsage mutUsage(this->cliMut);
		if (this->cli)
		{
			this->cli->Close();
		}
	}
	this->threadEvt.Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

void Net::ProtoClient::Start()
{
	if (this->started)
		return;
	this->started = true;
	this->threadEvt.Set();
}
void Net::ProtoClient::Reconnect()
{
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		this->cli->Close();
	}
}

Bool Net::ProtoClient::IsConnected()
{
	return this->connected;//this->cli != 0;
}

Bool Net::ProtoClient::SendPacket(UInt8 *buff, OSInt buffSize, Int32 cmdType, Int32 seqId)
{
	UInt8 sendBuff[2048];
	OSInt sendSize;
	Bool succ = true;
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		sendSize = this->proto->BuildPacket(sendBuff, cmdType, seqId, buff, buffSize, 0);
		if (this->cli->Write(sendBuff, sendSize) != sendSize)
		{
			succ = false;
		}
	}
	else
	{
		succ = false;
	}
	return succ;
}

Bool Net::ProtoClient::SendPacket(UInt8 *buff, OSInt buffSize)
{
	Bool succ = true;
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli)
	{
		if (this->cli->Write(buff, buffSize) != buffSize)
		{
			succ = false;
		}
	}
	else
	{
		succ = false;
	}
	return succ;
}
