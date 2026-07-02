#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ProtoClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

UInt32 __stdcall Net::ProtoClient::ProtoThread(AnyType userObj)
{
	NN<Net::ProtoClient> me = userObj.GetNN<Net::ProtoClient>();
	UInt8 buff[4096];
	IntOS buffSize;
	IntOS readSize;
	NN<Net::TCPClient> cli;
	
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		if (me->started && me->cli.IsNull())
		{
			Sync::MutexUsage mutUsage(me->cliMut);
			NEW_CLASSNN(cli, Net::TCPClient(me->sockf, me->cliAddr, me->cliPort, 30000));
			if (cli->IsConnectError())
			{
				cli.Delete();
				me->cli = nullptr;
			}
			else
			{
				me->cliData = me->proto->CreateStreamData(cli);
				me->cli = cli;
				buffSize = 0;
				me->connected = true;
				mutUsage.EndUse();
				me->cliHdlr->ClientConn();
			}
		}
		if (me->cli.SetTo(cli))
		{
			readSize = cli->Read(Data::ByteArray(&buff[buffSize], 2048));
			if (readSize == 0)
			{
				Sync::MutexUsage mutUsage(me->cliMut);
				me->proto->DeleteStreamData(cli, me->cliData);
				me->cli.Delete();
				me->connected = false;
				mutUsage.EndUse();
				me->cliHdlr->ClientDisconn();
			}
			else
			{
				buffSize += readSize;
				readSize = me->proto->ParseProtocol(cli, 0, me->cliData, Data::ByteArrayR(buff, buffSize));
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

		if (me->cli.IsNull())
		{
			me->threadEvt.Wait(10000);
		}
	}
	if (me->cli.SetTo(cli))
	{
		Sync::MutexUsage mutUsage(me->cliMut);
		me->proto->DeleteStreamData(cli, me->cliData);
		cli.Delete();
		me->cli = nullptr;
		me->connected = false;
		mutUsage.EndUse();
		me->cliHdlr->ClientDisconn();
	}
	me->threadRunning = false;
	return 0;
}

Net::ProtoClient::ProtoClient(NN<Net::SocketFactory> sockf, Text::CStringNN cliAddr, UInt16 cliPort, IO::ProtocolHandler *proto, Net::ProtoClient::IProtoClientHandler *cliHdlr)
{
	this->sockf = sockf;
	this->cli = nullptr;
	sockf->DNSResolveIP(cliAddr, this->cliAddr);
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
	NN<Net::TCPClient> cli;
	this->threadToStop = true;
	{
		Sync::MutexUsage mutUsage(this->cliMut);
		if (this->cli.SetTo(cli))
		{
			cli->Close();
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
	NN<Net::TCPClient> cli;
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli.SetTo(cli))
	{
		cli->Close();
	}
}

Bool Net::ProtoClient::IsConnected()
{
	return this->connected;//this->cli != 0;
}

Bool Net::ProtoClient::SendPacket(UInt8 *buff, IntOS buffSize, Int32 cmdType, Int32 seqId)
{
	UInt8 sendBuff[2048];
	IntOS sendSize;
	Bool succ = true;
	NN<Net::TCPClient> cli;
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli.SetTo(cli))
	{
		sendSize = this->proto->BuildPacket(sendBuff, cmdType, seqId, buff, buffSize, 0);
		if (cli->Write(Data::ByteArrayR(sendBuff, sendSize)) != sendSize)
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

Bool Net::ProtoClient::SendPacket(UInt8 *buff, IntOS buffSize)
{
	Bool succ = true;
	NN<Net::TCPClient> cli;
	Sync::MutexUsage mutUsage(this->cliMut);
	if (this->cli.SetTo(cli))
	{
		if (cli->Write(Data::ByteArrayR(buff, buffSize)) != buffSize)
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
