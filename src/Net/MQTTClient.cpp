#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Manage/HiResClock.h"
#include "Net/MQTTClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

void Net::MQTTClient::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	if ((cmdType & 0xf0) == 0x30 && cmdSize >= 2)
	{
		UInt8 qosLev = (cmdType & 6) >> 1;
		UOSInt i;
		UOSInt packetId = ReadMUInt16(&cmd[0]);
		UTF8Char *topic = 0;
		if ((UOSInt)(packetId + 2) <= cmdSize)
		{
			topic = MemAlloc(UTF8Char, packetId + 1);
			MemCopyNO(topic, &cmd[2], packetId);
			topic[packetId] = 0;
		}
		i = packetId + 2;
		if (qosLev == 1 || qosLev == 2)
		{
			if (i + 2 <= cmdSize)
			{
				packetId = ReadMUInt16(&cmd[i]);
			}
			else
			{
				packetId = 0;
			}
			i += 2;
		}
		if (topic && i <= cmdSize)
		{
			this->OnPublishMessage(topic, &cmd[i], cmdSize - i);
		}
		if (qosLev == 1)
		{
			this->SendPubAck((UInt16)packetId);
		}
		else if (qosLev == 2)
		{
			this->SendPubRec((UInt16)packetId);
		}
		if (topic)
		{
			MemFree(topic);
			topic = 0;
		}
	}
	else
	{
		PacketInfo *packet;
		packet = (PacketInfo*)MemAlloc(UInt8, sizeof(PacketInfo) + cmdSize);
		packet->packetType = (UInt8)cmdType;
		packet->size = cmdSize;
		MemCopyNO(packet->content, cmd, cmdSize);
		Sync::MutexUsage mutUsage(this->packetMut);
		this->packetList->Add(packet);
		mutUsage.EndUse();
		this->packetEvt->Set();
	}
}

void Net::MQTTClient::DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}

UInt32 __stdcall Net::MQTTClient::RecvThread(void *userObj)
{
	Net::MQTTClient *me = (Net::MQTTClient*)userObj;
	UInt8 buff[2048];
	UOSInt buffSize;
	UOSInt readSize;
	me->recvStarted = true;
	me->recvRunning = true;
	buffSize = 0;
	while (true)
	{
		readSize = me->cli->Read(&buff[buffSize], 2048 - buffSize);
		if (readSize <= 0)
			break;
		buffSize += readSize;
		readSize = me->protoHdlr->ParseProtocol(me->cli, me, me->cliData, buff, buffSize);
		if (readSize == 0)
		{
			buffSize = 0;
		}
		else if (readSize < buffSize)
		{
			MemCopyO(buff, &buff[buffSize - readSize], readSize);
			buffSize = readSize;
		}
	}
	me->recvRunning = false;
	return 0;
}

void Net::MQTTClient::OnPublishMessage(const UTF8Char *topic, const UInt8 *message, UOSInt msgSize)
{
	UOSInt i = this->hdlrList->GetCount();
	while (i-- > 0)
	{
		this->hdlrList->GetItem(i)(this->hdlrObjList->GetItem(i), topic, message, msgSize);
	}
}

Net::MQTTClient::PacketInfo *Net::MQTTClient::GetNextPacket(UInt8 packetType, UOSInt timeoutMS)
{
	Manage::HiResClock clk;
	PacketInfo *packet;
	Int64 t;
	while (true)
	{
		while (this->packetList->GetCount() > 0)
		{
			Sync::MutexUsage mutUsage(this->packetMut);
			packet = this->packetList->RemoveAt(0);
			mutUsage.EndUse();
			if ((packet->packetType & 0xf0) == packetType)
			{
				return packet;
			}
			MemFree(packet);
		}
		t = clk.GetTimeDiffus() / 1000;
		if (t >= (OSInt)timeoutMS)
			return 0;
		this->packetEvt->Wait(timeoutMS - (UOSInt)t);
	}
}

Net::MQTTClient::MQTTClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const Net::SocketUtil::AddressInfo *addr, UInt16 port, Bool sslConn)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->recvRunning = false;
	this->recvStarted = false;
	NEW_CLASS(this->hdlrList, Data::ArrayList<PublishMessageHdlr>());
	NEW_CLASS(this->hdlrObjList, Data::ArrayList<void *>());

	NEW_CLASS(this->packetMut, Sync::Mutex());
	NEW_CLASS(this->packetList, Data::ArrayList<PacketInfo*>());
	NEW_CLASS(this->packetEvt, Sync::Event(true, (const UTF8Char*)"Net.MQTTClient.packetEvt"));
	NEW_CLASS(this->protoHdlr, IO::ProtoHdlr::ProtoMQTTHandler(this));
	if (this->ssl && sslConn)
	{
		Net::SSLEngine::ErrorType err;
		UTF8Char sbuff[128];
		Net::SocketUtil::GetAddrName(sbuff, addr);
		this->cli = this->ssl->Connect(sbuff, port, &err);
	}
	else
	{
		NEW_CLASS(this->cli, Net::TCPClient(sockf, addr, port));
	}
	if (this->cli == 0)
	{

	}
	else if (this->cli->IsConnectError() != 0)
	{
		DEL_CLASS(this->cli);
		this->cli = 0;
	}
	else
	{
		this->cli->SetNoDelay(true);
		this->cliData = this->protoHdlr->CreateStreamData(this->cli);
		Sync::Thread::Create(RecvThread, this);
		while (!this->recvStarted)
		{
			Sync::Thread::Sleep(1);
		}
	}
}

Net::MQTTClient::~MQTTClient()
{
	if (this->cli)
	{
		if (this->recvRunning)
		{
			this->cli->Close();
		}
		while (this->recvRunning)
		{
			Sync::Thread::Sleep(1);
		}
		this->protoHdlr->DeleteStreamData(this->cli, this->cliData);
		DEL_CLASS(this->cli);
		this->cli = 0;
	}
	DEL_CLASS(this->protoHdlr);
	UOSInt i;
	i = this->packetList->GetCount();
	while (i-- > 0)
	{
		MemFree(this->packetList->GetItem(i));
	}
	DEL_CLASS(this->hdlrList);
	DEL_CLASS(this->hdlrObjList);
	DEL_CLASS(this->packetList);
	DEL_CLASS(this->packetEvt);
	DEL_CLASS(this->packetMut);
}

void Net::MQTTClient::HandlePublishMessage(PublishMessageHdlr hdlr, void *userObj)
{
	this->hdlrObjList->Add(userObj);
	this->hdlrList->Add(hdlr);
}

Bool Net::MQTTClient::IsError()
{
	return this->cli == 0 || !this->recvRunning;
}

Bool Net::MQTTClient::SendConnect(UInt8 protoVer, UInt16 keepAliveS, const UTF8Char *clientId, const UTF8Char *userName, const UTF8Char *password)
{
	UInt8 packet1[512];
	UInt8 packet2[512];

	UOSInt i;
	UOSInt j;
	packet1[0] = 0;
	packet1[1] = 4;
	packet1[2] = 'M';
	packet1[3] = 'Q';
	packet1[4] = 'T';
	packet1[5] = 'T';
	packet1[6] = protoVer;
	packet1[7] = 2; //Flags;
	WriteMInt16(&packet1[8], keepAliveS);
	i = 10;
	j = Text::StrCharCnt(clientId);
	WriteMInt16(&packet1[i], j);
	MemCopyNO(&packet1[i + 2], clientId, j);
	i += j + 2;
	if (userName)
	{
		packet1[7] |= 0x80;
		j = Text::StrCharCnt(userName);
		WriteMInt16(&packet1[i], j);
		MemCopyNO(&packet1[i + 2], userName, j);
		i += j + 2;
	}
	if (password)
	{
		packet1[7] |= 0x40;
		j = Text::StrCharCnt(password);
		WriteMInt16(&packet1[i], j);
		MemCopyNO(&packet1[i + 2], password, j);
		i += j + 2;
	}
	j = this->protoHdlr->BuildPacket(packet2, 0x10, 0, packet1, i, this->cliData);
	return this->cli->Write(packet2, j) == j;
}

Bool Net::MQTTClient::SendPublish(const UTF8Char *topic, const UTF8Char *message)
{
	UInt8 packet1[512];
	UInt8 packet2[512];

	UOSInt i;
	UOSInt j;
	i = 0;

	j = Text::StrCharCnt(topic);
	WriteMInt16(&packet1[i], j);
	MemCopyNO(&packet1[i + 2], topic, j);
	i += j + 2;
	j = Text::StrCharCnt(message);
	MemCopyNO(&packet1[i], message, j);
	i += j;

	j = this->protoHdlr->BuildPacket(packet2, 0x30, 0, packet1, i, this->cliData);
	return this->cli->Write(packet2, j) == j;
}

Bool Net::MQTTClient::SendPubAck(UInt16 packetId)
{
	UInt8 packet1[16];
	UInt8 packet2[16];
	UOSInt j;

	WriteMInt16(&packet1[0], packetId);
	j = this->protoHdlr->BuildPacket(packet2, 0x40, 0, packet1, 2, this->cliData);
	return this->cli->Write(packet2, j) == j;
}

Bool Net::MQTTClient::SendPubRec(UInt16 packetId)
{
	UInt8 packet1[16];
	UInt8 packet2[16];
	UOSInt j;

	WriteMInt16(&packet1[0], packetId);
	j = this->protoHdlr->BuildPacket(packet2, 0x50, 0, packet1, 2, this->cliData);
	return this->cli->Write(packet2, j) == j;
}

Bool Net::MQTTClient::SendSubscribe(UInt16 packetId, const UTF8Char *topic)
{
	UInt8 packet1[512];
	UInt8 packet2[512];

	UOSInt i;
	UOSInt j;

	WriteMInt16(&packet1[0], packetId);
	i = 2;
	j = Text::StrCharCnt(topic);
	WriteMInt16(&packet1[i], j);
	MemCopyNO(&packet1[i + 2], topic, j);
	i += j + 2;
	packet1[i] = 0;
	i++;

	j = this->protoHdlr->BuildPacket(packet2, 0x82, 0, packet1, i, this->cliData);
	return this->cli->Write(packet2, j) == j;
}

Bool Net::MQTTClient::SendPing()
{
	UInt8 packet2[16];
	UOSInt j;
	j = this->protoHdlr->BuildPacket(packet2, 0xc0, 0, packet2, 0, this->cliData);
	return this->cli->Write(packet2, j) == j;
}

Bool Net::MQTTClient::SendDisconnect()
{
	UInt8 packet2[16];
	UOSInt j;
	j = this->protoHdlr->BuildPacket(packet2, 0xe0, 0, packet2, 0, this->cliData);
	return this->cli->Write(packet2, j) == j;
}

Net::MQTTClient::ConnectStatus Net::MQTTClient::WaitConnAck(UOSInt timeoutMS)
{
	PacketInfo *packet = this->GetNextPacket(0x20, timeoutMS);
	if (packet == 0)
		return Net::MQTTClient::CS_TIMEDOUT;

	Net::MQTTClient::ConnectStatus ret = (Net::MQTTClient::ConnectStatus)packet->content[1];
	MemFree(packet);
	return ret;
}

UInt8 Net::MQTTClient::WaitSubAck(UInt16 packetId, UOSInt timeoutMS)
{
	PacketInfo *packet = this->GetNextPacket(0x90, timeoutMS);
	if (packet == 0)
		return 0x80;

	UInt8 ret;
	if (packet->size < 3 || packetId != ReadMUInt16(&packet->content[0]))
	{
		ret = 0x80;
	}
	else
	{
		ret = packet->content[2];
	}
	MemFree(packet);
	return ret;
}

void Net::MQTTClient::ClearPackets()
{
	Sync::MutexUsage mutUsage(this->packetMut);
	LIST_FREE_FUNC(this->packetList, MemFree);
	this->packetList->Clear();
	mutUsage.EndUse();
}

Bool Net::MQTTClient::PublishMessage(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UTF8Char *username, const UTF8Char *password, const UTF8Char *topic, const UTF8Char *message)
{
	Net::MQTTClient *cli;
	UTF8Char sbuff[64];
	NEW_CLASS(cli, Net::MQTTClient(sockf, 0, addr, port, false));
	if (cli->IsError())
	{
		DEL_CLASS(cli);
		return false;
	}

	Bool succ = false;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Text::StrInt64(Text::StrConcat(sbuff, (const UTF8Char*)"sswrMQTT/"), dt.ToTicks());
	if (cli->SendConnect(4, 30, sbuff, username, password))
	{
		succ = (cli->WaitConnAck(30000) == Net::MQTTClient::CS_ACCEPTED);
	}
	if (succ)
	{
		succ = cli->SendPublish(topic, message);
		cli->SendDisconnect();
	}
	
	DEL_CLASS(cli);
	return succ;
}
