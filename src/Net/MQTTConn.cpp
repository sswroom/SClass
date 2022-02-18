#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Manage/HiResClock.h"
#include "Net/MQTTConn.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

//#define DEBUG_PRINT
#if defined(DEBUG_PRINT)
#ifdef _MSC_VER
#include <windows.h>
#include <stdio.h>
#define printf(fmt, ...) {Char sbuff[512]; sprintf(sbuff, fmt, __VA_ARGS__); OutputDebugStringA(sbuff);}
#elif defined(DEBUGCON)
#include <stdio.h>
#include <syslog.h>
#define printf(fmt, ...) {Char sbuff[512]; sprintf(sbuff, fmt, __VA_ARGS__); syslog(LOG_DEBUG, sbuff);}
#else
#include <stdio.h>
#endif
#endif

void Net::MQTTConn::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
#if defined(DEBUG_PRINT)
	printf("On MQTT packet: type = %x, size = %d\r\n", cmdType, (UInt32)cmdSize);
#endif
	UInt8 packetType = (UInt8)(cmdType >> 4);
	if (packetType == 3 && cmdSize >= 2)
	{
		UInt8 qosLev = (cmdType & 6) >> 1;
		UOSInt i;
		UOSInt packetId = ReadMUInt16(&cmd[0]);
		UTF8Char *topic = 0;
		UOSInt topicLen = 0;
		if ((UOSInt)(packetId + 2) <= cmdSize)
		{
			topic = MemAlloc(UTF8Char, packetId + 1);
			MemCopyNO(topic, &cmd[2], packetId);
			topic[packetId] = 0;
			topicLen = packetId;
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
			this->OnPublishMessage({topic, topicLen}, &cmd[i], cmdSize - i);
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
	else if (packetType == 2 || packetType == 4 || packetType == 9 || packetType == 11 || packetType == 13)
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

void Net::MQTTConn::DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}

UInt32 __stdcall Net::MQTTConn::RecvThread(void *userObj)
{
	Net::MQTTConn *me = (Net::MQTTConn*)userObj;
	UOSInt maxBuffSize = 9000;
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt readSize;
	me->recvStarted = true;
	me->recvRunning = true;
	buffSize = 0;
	buff = MemAlloc(UInt8, maxBuffSize);
	while (true)
	{
		readSize = me->cli->Read(&buff[buffSize], maxBuffSize - buffSize);
		if (readSize <= 0)
			break;
		me->totalDownload += readSize;
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
	MemFree(buff);
	me->recvRunning = false;
	me->packetEvt->Set();
	if (me->discHdlr)
	{
		me->discHdlr(me->discHdlrObj);
	}
	return 0;
}

void Net::MQTTConn::OnPublishMessage(Text::CString topic, const UInt8 *message, UOSInt msgSize)
{
	UOSInt i = this->hdlrList->GetCount();
	while (i-- > 0)
	{
		this->hdlrList->GetItem(i)(this->hdlrObjList->GetItem(i), topic, message, msgSize);
	}
}

Net::MQTTConn::PacketInfo *Net::MQTTConn::GetNextPacket(UInt8 packetType, UOSInt timeoutMS)
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
		if (!this->recvRunning || t >= (OSInt)timeoutMS)
			return 0;
		this->packetEvt->Wait(timeoutMS - (UOSInt)t);
	}
}

Bool Net::MQTTConn::SendPacket(const UInt8 *packet, UOSInt packetSize)
{
	if (this->cli == 0)
	{
		return false;
	}
	UOSInt sendSize = this->cli->Write(packet, packetSize);
	this->totalUpload += sendSize;
	return sendSize == packetSize;
}

Net::MQTTConn::MQTTConn(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, DisconnectHdlr discHdlr, void *discHdlrObj)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->recvRunning = false;
	this->recvStarted = false;
	this->totalDownload = 0;
	this->totalUpload = 0;
	this->discHdlr = discHdlr;
	this->discHdlrObj = discHdlrObj;
	NEW_CLASS(this->hdlrList, Data::ArrayList<PublishMessageHdlr>());
	NEW_CLASS(this->hdlrObjList, Data::ArrayList<void *>());

	NEW_CLASS(this->packetMut, Sync::Mutex());
	NEW_CLASS(this->packetList, Data::ArrayList<PacketInfo*>());
	NEW_CLASS(this->packetEvt, Sync::Event(true));
	NEW_CLASS(this->protoHdlr, IO::ProtoHdlr::ProtoMQTTHandler(this));

	if (this->ssl)
	{
		Net::SSLEngine::ErrorType err;
		this->sockf->ReloadDNS();
		this->cli = this->ssl->Connect(host, port, &err);
#ifdef DEBUG_PRINT
	printf("MQTTConn: Connect to MQTTS: err = %d\r\n", (UInt32)err);
#endif
	}
	else
	{
		NEW_CLASS(this->cli, Net::TCPClient(sockf, host, port));
	}
	if (this->cli == 0)
	{

	}
	else if (this->cli->IsConnectError() != 0)
	{
		DEL_CLASS(this->cli);
		this->cli = 0;
#ifdef DEBUG_PRINT
		printf("MQTTConn connect error, %d\r\n", 0);
#endif
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

Net::MQTTConn::~MQTTConn()
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

void Net::MQTTConn::HandlePublishMessage(PublishMessageHdlr hdlr, void *userObj)
{
	this->hdlrObjList->Add(userObj);
	this->hdlrList->Add(hdlr);
}

Bool Net::MQTTConn::IsError()
{
	return this->cli == 0 || !this->recvRunning;
}

Bool Net::MQTTConn::SendConnect(UInt8 protoVer, UInt16 keepAliveS, Text::CString clientId, Text::CString userName, Text::CString password)
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
	j = clientId.leng;
	WriteMInt16(&packet1[i], j);
	MemCopyNO(&packet1[i + 2], clientId.v, j);
	i += j + 2;
	if (userName.leng > 0)
	{
		packet1[7] |= 0x80;
		j = userName.leng;
		WriteMInt16(&packet1[i], j);
		MemCopyNO(&packet1[i + 2], userName.v, j);
		i += j + 2;
	}
	if (password.leng > 0)
	{
		packet1[7] |= 0x40;
		j = password.leng;
		WriteMInt16(&packet1[i], j);
		MemCopyNO(&packet1[i + 2], password.v, j);
		i += j + 2;
	}
	j = this->protoHdlr->BuildPacket(packet2, 0x10, 0, packet1, i, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendPublish(Text::CString topic, Text::CString message)
{
	UInt8 packet1[512];
	UInt8 packet2[512];

	UOSInt i;
	UOSInt j;
	i = 0;

	j = topic.leng;
	WriteMInt16(&packet1[i], j);
	MemCopyNO(&packet1[i + 2], topic.v, j);
	i += j + 2;
	j = message.leng;
	MemCopyNO(&packet1[i], message.v, j);
	i += j;

	j = this->protoHdlr->BuildPacket(packet2, 0x30, 0, packet1, i, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendPubAck(UInt16 packetId)
{
	UInt8 packet1[16];
	UInt8 packet2[16];
	UOSInt j;

	WriteMInt16(&packet1[0], packetId);
	j = this->protoHdlr->BuildPacket(packet2, 0x40, 0, packet1, 2, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendPubRec(UInt16 packetId)
{
	UInt8 packet1[16];
	UInt8 packet2[16];
	UOSInt j;

	WriteMInt16(&packet1[0], packetId);
	j = this->protoHdlr->BuildPacket(packet2, 0x50, 0, packet1, 2, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendSubscribe(UInt16 packetId, Text::CString topic)
{
	UInt8 packet1[512];
	UInt8 packet2[512];

	UOSInt i;
	UOSInt j;

	WriteMInt16(&packet1[0], packetId);
	i = 2;
	j = topic.leng;
	WriteMInt16(&packet1[i], j);
	MemCopyNO(&packet1[i + 2], topic.v, j);
	i += j + 2;
	packet1[i] = 0;
	i++;
#if defined(DEBUG_PRINT)
	printf("Subscribing topic %s\r\n", topic);
#endif
	j = this->protoHdlr->BuildPacket(packet2, 0x82, 0, packet1, i, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendPing()
{
	UInt8 packet2[16];
	UOSInt j;
	j = this->protoHdlr->BuildPacket(packet2, 0xc0, 0, 0, 0, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendDisconnect()
{
	UInt8 packet2[16];
	UOSInt j;
	j = this->protoHdlr->BuildPacket(packet2, 0xe0, 0, 0, 0, this->cliData);
	return this->SendPacket(packet2, j);
}

Net::MQTTConn::ConnectStatus Net::MQTTConn::WaitConnAck(UOSInt timeoutMS)
{
	PacketInfo *packet = this->GetNextPacket(0x20, timeoutMS);
	if (packet == 0)
		return Net::MQTTConn::CS_TIMEDOUT;

	Net::MQTTConn::ConnectStatus ret = (Net::MQTTConn::ConnectStatus)packet->content[1];
	MemFree(packet);
	return ret;
}

UInt8 Net::MQTTConn::WaitSubAck(UInt16 packetId, UOSInt timeoutMS)
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

void Net::MQTTConn::ClearPackets()
{
	Sync::MutexUsage mutUsage(this->packetMut);
	LIST_FREE_FUNC(this->packetList, MemFree);
	this->packetList->Clear();
	mutUsage.EndUse();
}

UInt64 Net::MQTTConn::GetTotalUpload()
{
	return this->totalUpload;
}

UInt64 Net::MQTTConn::GetTotalDownload()
{
	return this->totalDownload;
}

Bool Net::MQTTConn::PublishMessage(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Text::CString username, Text::CString password, Text::CString topic, Text::CString message)
{
	Net::MQTTConn *cli;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	NEW_CLASS(cli, Net::MQTTConn(sockf, ssl, host, port, 0, 0));
	if (cli->IsError())
	{
		DEL_CLASS(cli);
		return false;
	}

	Bool succ = false;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	sptr = Text::StrInt64(Text::StrConcatC(sbuff, UTF8STRC("sswrMQTT/")), dt.ToTicks());
	if (cli->SendConnect(4, 30, CSTRP(sbuff, sptr), username, password))
	{
		succ = (cli->WaitConnAck(30000) == Net::MQTTConn::CS_ACCEPTED);
	}
	if (succ)
	{
		succ = cli->SendPublish(topic, message);
		cli->SendDisconnect();
	}
	
	DEL_CLASS(cli);
	return succ;
}
